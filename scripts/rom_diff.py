#!/bin/env python3

import argparse
from glob import glob
from os import makedirs, path
from re import sub
from shlex import quote
from subprocess import getoutput
from typing import Dict, Tuple, Union


def config():
    return None


config.size_width = 9
config.default_no_value_str = "-"
config.units = ["", "k", "M", "G"]
config.unit_multiple = 1024
config.extension = ".files"


def convert_human_readable(size: int, mult: int, units: list) -> Tuple[float, str]:
    unit_index = 0

    while abs(size) >= mult:
        unit_index += 1
        size /= mult

    return size, units[unit_index]


def convert_dummy(size: int, _: str, __: bool = False) -> Tuple[float, str]:
    return float(size), ""


def format_size(size: int, unit: str, add_sign: bool = False) -> str:
    format_str = "{:+.2f}" if add_sign else "{:.2f}"

    return sub("\.0+", "", format_str.format(size)) + unit


def print_line(
    l_file: str,
    l_size: Union[str, int],
    size_diff: int,
    r_size: Union[str, int],
    r_file: str,
    config: callable,
) -> None:
    if isinstance(l_size, int):
        l_size = format_size(
            *config.convert_f(l_size, config.unit_multiple, config.units)
        )
    if isinstance(r_size, int):
        r_size = format_size(
            *config.convert_f(r_size, config.unit_multiple, config.units)
        )
    if isinstance(size_diff, int) and size_diff != 0:
        size_diff = format_size(
            *config.convert_f(size_diff, config.unit_multiple, config.units), True
        )

    print(
        f"{l_file: >{config.file_width}} {l_size: >{config.size_width}} {size_diff:>{config.size_width}} {r_size: >{config.size_width}} {r_file}"
    )


def parse_file(filepath: str) -> Dict[str, str]:
    with open(filepath, "r") as f:
        return dict([line.strip().split("\t")[::-1] for line in f])


def get_stat(dictionary: Dict[str, str], key: str, default_val: str) -> Tuple[str, int]:
    if key in dictionary:
        return key, int(dictionary[key])

    return default_val, 0


def compare(args: object, config: callable) -> None:
    config.convert_f = convert_human_readable if args.human_readable else convert_dummy

    old_stats = parse_file(config.logs_dir + args.old_version + config.extension)
    new_stats = parse_file(config.logs_dir + args.new_version + config.extension)

    all_files = sorted(set(list(old_stats.keys()) + list(new_stats.keys())))
    config.file_width = max([len(file) for file in all_files])

    print_line("Old file", "Size", "Diff", "New file", "Size", config)

    for f in all_files:
        l_file, l_size = get_stat(old_stats, f, config.default_no_value_str)
        r_file, r_size = get_stat(new_stats, f, config.default_no_value_str)

        size_diff = r_size - l_size
        if size_diff == 0:
            if not args.all:
                continue

        if l_file == config.default_no_value_str:
            l_size = config.default_no_value_str
        if r_file == config.default_no_value_str:
            r_size = config.default_no_value_str

        print_line(l_file, l_size, size_diff, r_size, r_file, config)


def list_versions(args: object, config: callable) -> None:
    print(
        *sorted(
            [
                sub("\\" + config.extension + "$", "", path.basename(f))
                for f in glob(config.logs_dir + "*" + config.extension)
            ]
        ),
        sep="\n",
    )


def save(args: object, config: callable) -> None:
    root_dir = quote(args.info_path)

    if path.isdir(root_dir):
        files_stats = getoutput(
            f"cd '{root_dir}' && du --all --bytes . | while IFS='\t' read -r size line; do printf '%s\\t%s' \"$size\" \"${{line#.}}\"; test -d \"$line\" && printf '/'; echo; done | sort -k2"
        )
    elif path.isfile(root_dir):
        print(f"{args.root_info_pathdir}: not a directory")
        exit(1)
    else:
        print(f"{args.root_info_pathdir}: no such file or directory")
        exit(1)

    f_squashfs = getoutput(
        f"find '{path.dirname(root_dir)}' -type f -name 'root.squashfs' | head -n1"
    )
    f_kernel = getoutput(
        f"find '{path.dirname(f_squashfs)}' -maxdepth 1 -type f -name '*.bin' | head -n1"
    )

    files_stats = (
        f"{path.getsize(f_kernel)}\t{path.basename(f_kernel)}\n"
        + f"{path.getsize(f_squashfs)}\t{path.basename(f_squashfs)}\n"
        + files_stats
        + "\n"
    )

    output_file = config.logs_dir + config.current_tag + config.extension
    with open(output_file, "w") as file:
        file.write(files_stats)

    print(f"Info about files in FW written to {output_file}")


def parse_args() -> object:
    parser = argparse.ArgumentParser(
        description="Show / collect information about files' differences between FW versions"
    )

    parser.add_argument(
        "target",
        help="Compilation target (or 'update' to pull files required for comparison from the repository). For a list of available targets, use './select.sh'",
    )

    subparsers = parser.add_subparsers(title="actions")

    parser_compare = subparsers.add_parser(
        "compare",
        aliases=["c"],
        help="Compare FW file differences between versions",
    )
    parser_compare.add_argument(
        "-a", "--all", action="store_true", help="Don't skip files that are the same"
    )
    parser_compare.add_argument(
        "-H",
        "--human-readable",
        action="store_true",
        help="Print sizes in human readable format (e.g., 1k 2.5M 2G)",
    )
    parser_compare.add_argument(
        "old_version",
        help="Old ROM file list to use as a base for comparison",
    )
    parser_compare.add_argument(
        "new_version",
        help="Any ROM file list newer than <old_version>",
    )
    parser_compare.set_defaults(func=compare)

    parser_list = subparsers.add_parser(
        "list", aliases=["l"], help="List FW versions available for comparison"
    )
    parser_list.set_defaults(func=list_versions)

    parser_save = subparsers.add_parser(
        "save", aliases=["s"], help="Save information about the FW file system"
    )
    parser_save.add_argument(
        "info_path",
        help="A root directory of device files (ROM), like build_dir/target-*/root-*",
    )
    parser_save.set_defaults(func=save)

    return parser.parse_args()


args = parse_args()

basedir = f"{path.dirname(__file__)}/rom-diff"

if args.target == "update":
    print(getoutput(f"git submodule update --init {basedir}"))
    print(
        getoutput(
            f"git -C '{basedir}' checkout $(git config --file=.gitmodules submodule.\"$(git submodule | awk '{{print $2}}')\".branch)"
        )
    )
    print(getoutput(f"git -C '{basedir}' pull"))
    exit(0)


config.logs_dir = path.normpath(f"{basedir}/{args.target}") + "/"
makedirs(config.logs_dir, exist_ok=True)

config.current_tag = getoutput("git describe --tags --abbrev=0")
commit_count = int(getoutput(f"git rev-list {config.current_tag}..HEAD --count"))
if commit_count > 0:
    config.current_tag += f"-{commit_count}"

args.func(args, config)
