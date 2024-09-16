#!/usr/bin/env bash

[ "$#" -ne 4 ] && {
	echo "Usage prepare_tlt_pm_package.sh <packages> <top_dir> <arch> <package_dir>"
	exit 1
}

read -ra packages <<<"$1"
top_dir=$2
arch=$3

count=${#packages[@]}
# No packages provided means we can quietly exit
[[ $count -le 0 ]] && exit 0

parent_dir="$top_dir/bin/packages/$arch"
declare -A dirs
dirs[base_dep]="$parent_dir/base"
dirs[vuci_dep]="$parent_dir/vuci"
dirs[packages_dep]="$parent_dir/packages"
dirs[kmod_dep]="${4}"

dirs[pm_packages]="$parent_dir/pm_packages"
dirs[zipped_packages]="$parent_dir/zipped_packages"

dirs[tmp_pm]="$top_dir/tmp/pm"

json_file="$top_dir/ipk_packages.json"

cd "${dirs[pm_packages]}" || exit 1

mkdir -p "${dirs[tmp_pm]}"

# Kill child processes and cleanup on interrupts
trap 'kill -TERM "${pids[@]}"; rm -fr "${dirs[tmp_pm]}"; exit 1' HUP INT TERM
trap 'rm -fr "${dirs[tmp_pm]}"' EXIT

err() {
	local fmt=$1
	shift 1
	# shellcheck disable=2059
	echo -e "\x1B[31m$(printf "$fmt" "$@")\x1B[0m" >&2
}

prepare_control_f() {
	local package=$1
	local name="$2"

	mkdir -p "${dirs[tmp_pm]}/$name"

	main_f="${dirs[tmp_pm]}/$name/main"
	touch "$main_f"

	tar -xf "$package" --get "./control.tar.gz" --to-stdout |
		tar -xzf - --get "./control" --to-stdout |
		grep -E 'Firmware|tlt_name|Router|Package|Version|pkg_reboot|pkg_network_restart' >"$main_f" &&
		tar -uf "${dirs[zipped_packages]}/$name.tar" -C "${dirs[tmp_pm]}/$name" "./main"

	rm -r "${dirs[tmp_pm]:?}/$name"
}

unpack_ipk() {
	local pkg=$1
	local dest=$2

	mkdir -p "$dest/CONTROL"
	tar -xzf "$pkg" --get "./control.tar.gz" --to-stdout | tar -xzf - -C "$dest/CONTROL" || return $?
	tar -xzf "$pkg" --get "./data.tar.gz" --to-stdout | tar -xzf - -C "$dest" || return $?
}

repackage_and_sign() {
	local name=$1
	local pkg=$2
	local dest=$3
	local tmp_pkg_dir

	printf "%-33s: " "$name"

	tmp_pkg_dir=$(mktemp "${dirs[tmp_pm]}/ipkg.XXXX")
	rm "$tmp_pkg_dir"
	mkdir -p "$tmp_pkg_dir" # no need to rm, will be cleaned up by exit trap
	unpack_ipk "$pkg" "$tmp_pkg_dir" || {
		err "%s exited with code %d" "unpack_ipk" $?
		return 1
	}
	"$top_dir/scripts/ipkg-build" -m "" "$tmp_pkg_dir" "$dest" | sed -e "s|$top_dir/||g" || {
		err "%s exited with code %d" "ipkg-build" $?
		return 1
	}
}

find_ipk() {
	local name=$1
	local dir=$2

	# get filename of (find ipks | sort by length | take 1st (shortest)) | grep to return non-zero exit code when nothing found (empty string)
	basename "$(find "$dir/" -type f -name "${name}_*.ipk" | perl -e 'print sort { length($a) <=> length($b) } <>' | head -n1)" | grep .
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

pack_deps() {
	local pkg=$1
	local dep_type=$2
	local dest_path=$3
	local tar_path=$4
	local d_fullname

	local ret=0

	for d in $(jq -r ".\"$pkg\".$dep_type" "$json_file"); do
		[ "$d" = "null" ] && break
		# # Skip packages that are included in the FW
		check_config "${d}" "y" && {
			printf "%-33s: skipped, included in FW\n" "$d"
			continue
		}

		pkg_dep_type=$dep_type

		# If dep not found in specified dir, search other dirs and display a warning if found
		d_fullname=$(find_ipk "$d" "${dirs[$dep_type]}/") || for dep_dir in "${!dirs[@]}"; do
			d_fullname=$(find_ipk "$d" "${dirs[$dep_dir]}/") || continue

			err "Warning: dependency '%s' for '%s' specified as '%s' but found in '%s'. Please fix" "$d" "$pkg" "$dep_type" "$dep_dir"
			pkg_dep_type=$dep_dir
			break
		done

		[ -z "$d_fullname" ] && {
			# If package exists but is not selected, only print a warning
			if check_config "$d" "n"; then
				err "%-33s: Warning: package is not compiled (=n), skipping" "$d"
			else
				err "%-33s: package not found" "$d"
				ret=$((ret + 1))
			fi
			continue
		}

		# Don't repackage already packaged ipks
		if find ./ -type f -name "$d_fullname" | grep -q .; then {
			printf "%-33s: Reused an already packaged %s\n" "$d" "$d_fullname"
		}; else {
			repackage_and_sign "$d" "${dirs[$pkg_dep_type]}/$d_fullname" "$dest_path" || return $?
		}; fi

		# If package already exists in archive, find_ipk most likely failed and returned a wrong package at some point
		tar -tf "$tar_path" ./"$d_fullname" &>/dev/null && {
			err "Error: Duplicate file '%s' for '%s' package in archive, either a duplicate dependency or a package search failure" "$d_fullname" "$d"
			ret=$((ret + 1))
			continue
		}
		tar -uf "$tar_path" ./"$d_fullname"
	done

	return $ret
}

pack_package() {
	local p=$1
	local out=$2
	local ret=0
	local name
	local tar_name

	[[ -n $out ]] && exec &>"$out"

	case "$p" in
	hs_theme* | vuci-i18n-*) source_dir="vuci_dep" ;;
	iptables-*) source_dir="kmod_dep" ;;
	*) source_dir="base_dep" ;;
	esac

	p_fullpath=$(find_ipk "$p" "${dirs[$source_dir]}")

	name=$(jq -r ".\"$p\".name" "$json_file")
	[ "$name" = "null" ] && name="$p"
	tar_name="${dirs[zipped_packages]}/$name.tar"

	printf "\nPackaged %s and its dependencies:\n" "$name"
	repackage_and_sign "$p" "${dirs[$source_dir]}/$p_fullpath" "${dirs[pm_packages]}" 2>/dev/null
	ret=$?
	if [[ $ret -ne 0 ]] && ! [[ -f "${dirs[pm_packages]}/$p_fullpath" ]]; then
		return $ret
	else
		ret=0
	fi

	tar -cf "$tar_name" ./"$p_fullpath"

	prepare_control_f "$p_fullpath" "$name"

	for dep_type in base_dep vuci_dep packages_dep kmod_dep; do
		pack_deps "$p" "$dep_type" "${dirs[pm_packages]}" "$tar_name"
		ret=$((ret + $?))
	done

	gzip "$tar_name"

	return $ret
}

parallel_jobs=$(echo " $MAKEFLAGS " | sed -Ee 's|(.*\s-j([0-9]+)\s)?.*|\2|g')
# Default value = 1 (-j1)
parallel_jobs=${parallel_jobs:-1}

chunk_size=$(((count + parallel_jobs - 1) / parallel_jobs)) # round up

declare -a pids
code_f_postfix=".pkg.ret"

if [[ $V == "" ]]; then
	# if verbose is turned off, don't bother reading logs
	get_list_of_finished_logs() { sleep 1; }
	log_f_postfix=
else
	get_list_of_finished_logs() {
		find "${dirs[tmp_pm]}" -type f -name "*$log_f_postfix" 2>/dev/null | while read -r f; do
			fuser -s "$f" || echo "$f"
		done
	}
	log_f_postfix=".pkg.log"
fi

# Suppress information about started background processes
set +m

for i in $(seq 0 "$((parallel_jobs - 1))"); do
	for p in "${packages[@]:$((i * chunk_size)):$chunk_size}"; do
		pack_package "$p" "${log_f_postfix:+${dirs[tmp_pm]}/${p}${log_f_postfix}}" # use files as output buffers to minimize race conditions between threads
		echo $? >"${dirs[tmp_pm]}/${p}${code_f_postfix}"                           # save return code of each package to file
	done &
	pids+=("$!")
done

echo "${pids[@]}" >>childs.pid
sleep 5
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

	err "Encountered %d error(s) while packaging %s" "$ret_pkg" "$(basename "$ret_f" "$code_f_postfix")"
	ret_total=$((ret_total + ret_pkg))
done
echo

exit $ret_total
