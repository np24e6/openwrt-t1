#!/usr/bin/env bash

unpack=false
IGNORE_NOT_FOUND=false
declare -A dirs

usage="Usage: $0 [-u SIGN_FILE_LIST | -p] [-i] [-h] PACKAGES TOPDIR ARCH KMOD_IPK_PATH\n\nActions:\n\t-u\tUnpack and prepare\n\t-p\tPack\n\nOptions:\n\t-i\tIgnore packages that were not found\n"
#shellcheck disable=2059
while getopts "u:pih" opt; do
	case $opt in
	u)
		SIGN_FILE_LIST=$OPTARG
		error_str_fmt="Encountered %d error(s) while unpacking %s"
		unpack=true
		trap 'find "${dirs[tmp_pm]}" -maxdepth 1 ! -type d ! -name ".*" -exec rm {} \;' EXIT
		;;
	p)
		error_str_fmt="Encountered %d error(s) while packaging %s"
		trap 'rm -fr "${dirs[tmp_pm]}"' EXIT
		;;
	i) IGNORE_NOT_FOUND=true ;;
	h | \?) printf "$usage" >&2 && exit ;;
	esac
done

shift $((OPTIND - 1))

read -ra packages <<<"$1"
top_dir=$2
arch=$3

count=${#packages[@]}
# No packages provided means we can quietly exit
[[ $count -le 0 ]] && exit 0

parent_dir="$top_dir/bin/packages/$arch"
dirs[base_dep]="$parent_dir/base"
dirs[vuci_dep]="$parent_dir/vuci"
dirs[packages_dep]="$parent_dir/packages"
dirs[kmod_dep]="${4}"

original_pkg_dirs='base_dep vuci_dep packages_dep kmod_dep'

dirs[pm_packages]="$parent_dir/pm_packages"
dirs[zipped_packages]="$parent_dir/zipped_packages"

dirs[tmp_pm]="$top_dir/tmp/pm"

json_file="$top_dir/ipk_packages.json"

cd "${dirs[pm_packages]}" || exit 1

mkdir -p "${dirs[tmp_pm]}"

$unpack && {
	[[ -z $SIGN_FILE_LIST ]] && {
		err 'SIGN_FILE_LIST is required when unpacking'
		exit 1
	}
	# Always use absolute path
	[[ "$SIGN_FILE_LIST" = /* ]] || SIGN_FILE_LIST=$top_dir/$SIGN_FILE_LIST
	mkdir -p "$(dirname "$SIGN_FILE_LIST")"
	echo -n '' >"$SIGN_FILE_LIST"
}

# Kill child processes and cleanup on interrupts
trap 'kill -TERM "${pids[@]}"; rm -fr "${dirs[tmp_pm]}"; exit 1' HUP INT TERM

err() {
	local fmt=$1
	shift 1
	# shellcheck disable=2059
	echo -e "\x1B[31m$(printf "$fmt" "$@")\x1B[0m" >&2
}

prepare_control_f() {
	local ipk_name=$1
	local pm_name="$2"
	local package_name=$3

	mkdir -p "${dirs[tmp_pm]}/$package_name"

	main_f="${dirs[tmp_pm]}/$package_name/main"
	touch "$main_f"

	tar -xf "$ipk_name" --get "./control.tar.gz" --to-stdout |
		tar -xzf - --get "./control" --to-stdout |
		grep -E 'Firmware|tlt_name|Router|Package|Version|pkg_reboot|pkg_network_restart|AppName' >"$main_f" &&
		tar -uf "${dirs[zipped_packages]}/$pm_name.tar" -C "${dirs[tmp_pm]}/$package_name" "./main"

	rm -r "${dirs[tmp_pm]:?}/$package_name"
}

unpack_ipk() {
	local pkg=$1
	local dest=$2

	mkdir -p "$dest/CONTROL"
	tar -xzf "$pkg" --get "./control.tar.gz" --to-stdout | tar -xzf - -C "$dest/CONTROL" || return $?
	tar -xzf "$pkg" --get "./data.tar.gz" --to-stdout | tar -xzf - -C "$dest" || return $?
}

unpack_and_prepare() {
	local name=$1
	local pkg=$2
	local dest=$3
	local tmp_pkg_dir=${dirs[tmp_pm]}/$name

	printf "%-33s: " "$name"

	[[ -f "$dest/$name/control+data.tmp" ]] && {
		printf "Already prepared for signing\n"
		return 0
	}

	# shellcheck disable=2010
	ls -A1q "$tmp_pkg_dir/CONTROL" 2>/dev/null | grep -q . || {
		mkdir -p "$tmp_pkg_dir" # no need to rm, will be cleaned up by exit trap if necessary
		unpack_ipk "$pkg" "$tmp_pkg_dir" || {
			err "unpack_ipk exited with code %d" $?
			return 1
		}
	}

	"$top_dir/scripts/ipkg-build" -u "$SIGN_FILE_LIST" -m "" "$tmp_pkg_dir" "$dest" || {
		err "ipkg-build exited with code %d" $?
		rm -fr "$tmp_pkg_dir"
		return 1
	}
}

pack() {
	local d=$1
	local d_fullname=$2
	local dest_path=$3
	local tar_path=$4

	printf "%-33s: " "$d"

	# Don't process already processed ipks
	if find_ipk "$d_fullname" ./ >/dev/null; then
		printf "Reused an already packaged %s\n" "$d_fullname"
	else
		"$top_dir/scripts/ipkg-build" -p -m "" "${dirs[tmp_pm]}/$d" "$dest_path" || {
			err "ipkg-build exited with code %d" $?
			return 1
		}
	fi

	[[ -z "$tar_path" ]] && return

	# If package already exists in archive, find_ipk most likely failed and returned a wrong package at some point
	tar -tf "$tar_path" ./"$d_fullname" &>/dev/null && {
		err "Error: Duplicate file '%s' in archive %s, either a duplicate dependency or a package search failure" "$d_fullname" "$tar_path"
		return 1
	}
	tar -uf "$tar_path" ./"$d_fullname"
}

find_ipk() {
	local name=$1
	local dir=$2

	# Allow specifying both partial package name and full ipk filename
	echo "$name" | grep -qE '\.ipk$' || name="${name}_*.ipk"

	# get filename of (find ipks | sort by length | take 1st (shortest)) | grep to return non-zero exit code when nothing found (empty string)
	basename "$(find "$dir/" -type f -name "$name" | perl -e 'print sort { length($a) <=> length($b) } <>' | head -n1)" | grep .
}

check_config() {
	local pkg=$1
	local state=$2

	case $state in
	m | y) state="=$state" ;;
	n | *) state=" is not set" ;;
	esac

	grep -q "CONFIG_PACKAGE_${pkg}$state" "$top_dir/.config"
}

find_dep_fullname() {
	local d=$1
	local specified_dir=$2
	local d_ipk_var=$3
	local dep_dir_var=$4
	local d_ipk

	# If dep not found in specified dir, search other dirs and display a warning if found
	if d_ipk=$(find_ipk "$d" "$specified_dir/"); then
		export "$d_ipk_var=$d_ipk"
		return 0
	else
		for dep_dir in $original_pkg_dirs; do
			d_ipk=$(find_ipk "$d" "${dirs[$dep_dir]}/") || continue
			export "$d_ipk_var=$d_ipk"
			export "$dep_dir_var=$dep_dir"
			return 0
		done
	fi

	# If package exists but is not selected, only print a warning
	if $IGNORE_NOT_FOUND || check_config "$d" "n"; then
		err "%-33s: Warning: package is not compiled (=n), skipping" "$d"
		return 0
	else
		err "%-33s: package not found" "$d"
	fi

	return 1
}

process_deps() {
	local unpack=$1
	local pkg=$2
	local dep_type=$3
	local dest_path=$4
	local tar_path=$5

	local ret=0

	$unpack && cmd=unpack_and_prepare || cmd=pack

	for d in $(jq -r ".\"$pkg\".$dep_type" "$json_file"); do
		[[ $d = "null" ]] && break
		# # Skip packages that are included in the FW
		check_config "${d}" "y" && {
			printf "%-33s: skipped, included in FW\n" "$d"
			continue
		}

		d_fullname=''
		pkg_dep_type=$dep_type

		find_dep_fullname "$d" "${dirs[$dep_type]}" d_fullname pkg_dep_type
		local f_ret=$?
		[[ -z $d_fullname ]] && {
			ret=$((ret + f_ret))
			continue
		}

		if [[ $pkg_dep_type != "$dep_type" ]]; then
			err "Warning: dependency '%s' for '%s' specified as '%s' but found in '%s'. Please fix" "$d" "$pkg" "$dep_type" "$pkg_dep_type"
		fi
		$unpack && d_fullname=${dirs[$pkg_dep_type]}/$d_fullname

		$cmd "$d" "$d_fullname" "$dest_path" "$tar_path" || {
			ret=$((ret + $?))
			continue
		}
	done

	return $ret
}

process_package() {
	local unpack=$1
	local p=$2
	local out=$3
	local ret=0
	local name
	local tar_name

	[[ -n $out ]] && exec &>"$out"

	case "$p" in
	hs_theme* | vuci-*) source_dir="vuci_dep" ;;
	iptables-*) source_dir="kmod_dep" ;;
	*) source_dir="base_dep" ;;
	esac

	p_fullpath=$(find_ipk "$p" "${dirs[$source_dir]}")

	if $unpack; then
		printf "\nPrepared %s and its dependencies for signing:\n" "$p"
		unpack_and_prepare "$p" "${dirs[$source_dir]}/$p_fullpath" "${dirs[pm_packages]}" 2>/dev/null || return $?
	else
		name=$(jq -r ".\"$p\".name" "$json_file")
		[[ $name = "null" ]] && name="$p"
		tar_name="${dirs[zipped_packages]}/$name.tar"

		printf "\nPackaged %s:\n" "$name"
		pack "$p" "$p_fullpath" "${dirs[pm_packages]}"

		tar -cf "$tar_name" ./"$p_fullpath"
		prepare_control_f "$p_fullpath" "$name" "$p"
	fi

	for dep_type in $original_pkg_dirs; do
		process_deps "$unpack" "$p" "$dep_type" "${dirs[pm_packages]}" "$tar_name"
		ret=$((ret + $?))
	done

	$unpack || gzip "$tar_name"

	return $ret
}

####################################################################
# Multi-threaded (kind of) packing/unpacking and logging to stdout #
####################################################################

parallel_jobs=$(echo " $MAKEFLAGS " | sed -Ee 's|(.*\s-j([0-9]+)\s)?.*|\2|g')
# Default value = 1 (-j1)
parallel_jobs=${parallel_jobs:-1}

chunk_size=$(((count + parallel_jobs - 1) / parallel_jobs)) # round up

declare -a pids
code_f_postfix=".pkg.ret"
log_f_postfix=".pkg.log"

if [[ -z $V ]]; then
	# if verbose is turned off, don't bother reading logs
	get_list_of_finished_logs() { sleep 1; }
else
	get_list_of_finished_logs() {
		find "${dirs[tmp_pm]}" -type f -name "*$log_f_postfix" 2>/dev/null | while read -r f; do
			fuser -s "$f" 2>/dev/null || echo "$f"
		done
	}
fi

# Suppress information about started background processes
set +m

for i in $(seq 0 "$((parallel_jobs - 1))"); do
	for p in "${packages[@]:$((i * chunk_size)):$chunk_size}"; do
		process_package "$unpack" "$p" "${V:+${dirs[tmp_pm]}/${p}${log_f_postfix}}" # use files as output buffers to minimize race conditions between threads
		echo $? >"${dirs[tmp_pm]}/${p}${code_f_postfix}"                            # save return code of each package to file
	done &
	pids+=("$!")
done

echo "${pids[@]}" >>childs.pid
# Wait for child processes to finish and print their output
while [[ -n $log_list ]] || pgrep --parent "$$" --pidfile childs.pid >/dev/null; do
	log_list=$(get_list_of_finished_logs)
	# shellcheck disable=2086 # splitting is intended here
	[[ -n $log_list ]] && cat $log_list && rm $log_list
done

rm childs.pid

echo
# Collect return codes from threads
ret_total=0
for ret_f in "${dirs[tmp_pm]}"/*"$code_f_postfix"; do
	ret_pkg=$(cat "$ret_f")
	[[ $ret_pkg == 0 ]] && continue

	err "$error_str_fmt" "$ret_pkg" "$(basename "$ret_f" "$code_f_postfix")"
	ret_total=$((ret_total + ret_pkg))
done
echo

exit $ret_total
