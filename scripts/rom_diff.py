#!/bin/env python3

import argparse
from glob import glob
from math import ceil
from os import getcwd, get_terminal_size, makedirs, path
from re import compile as re_compile
from shlex import quote
from shutil import rmtree
from subprocess import getoutput, getstatusoutput
from sys import stderr
from typing import Dict, Tuple, Union


class bcolors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def config():
    return None


def regex():
    return None


def exit_w_err(err: str, code: int = 1) -> None:
    print(err, file=stderr)
    exit(code)


config.size_width = 9
config.default_no_value_str = "-"
config.units = ["", "k", "M", "G"]
config.unit_multiple = 1024

regex.size = re_compile(r"\.0+$")
regex.numeric_only = re_compile(r"[^\d.]")


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

    return regex.size.sub("", format_str.format(size)) + unit


def fold_line(full_line: str, max_width: int) -> Tuple[str, str]:
    if len(full_line) <= max_width:
        return "", full_line

    line_1 = full_line[:max_width]
    slash = line_1.rfind("/") + 1
    index = slash if slash > 1 else max_width

    return full_line[:index], full_line[index:]


def print_line(
    l_file: str,
    l_size: Union[str, int],
    size_diff: int,
    r_size: Union[str, int],
    r_file: str,
    config: callable,
    color: bcolors = bcolors.ENDC,
) -> None:
    if isinstance(l_size, int):
        l_size = format_size(*config.convert_f(l_size, config.unit_multiple, config.units))
    if isinstance(r_size, int):
        r_size = format_size(*config.convert_f(r_size, config.unit_multiple, config.units))
    if isinstance(size_diff, int) and size_diff != 0:
        size_diff = format_size(*config.convert_f(size_diff, config.unit_multiple, config.units), True)

    l_file_part, l_file = fold_line(l_file, config.file_width)

    if l_file_part:
        print_line(l_file_part.ljust(config.file_width), "", "", "", "", config)
        print_line(l_file, l_size, size_diff, r_size, r_file, config, color)
        return

    r_file_part, r_file = fold_line(r_file, config.file_width)

    print(
        f"{l_file: >{config.file_width}} {l_size: >{config.size_width}} {color}{size_diff:>{config.size_width}}{bcolors.ENDC} {r_size: >{config.size_width}} {r_file_part if r_file_part else r_file: >{0 if r_size else config.file_width}}"
    )

    if r_file_part:
        print_line("", "", "", "", r_file.rjust(config.file_width), config)


def parse_file(version_str: str, config: callable) -> Dict[str, str]:
    try:
        with open(config.logs_dir + version_str + config.extension, "r") as f:
            return dict([line.strip().split("\t")[::-1] for line in f])
    except FileNotFoundError:
        exit_w_err(f"Information about version '{version_str}' does not exist. Try 'update' command.")


def get_stat(dictionary: Dict[str, str], key: str, default_val: str) -> Tuple[str, int]:
    if key in dictionary:
        return key, int(dictionary[key])

    return default_val, 0


def calc_file_width(size_width: int, files: list) -> int:
    file_width = max([len(file) for file in files])

    try:
        terminal_width = get_terminal_size().columns
    except OSError:
        return 30

    full_line_width = file_width * 2 + size_width * 3 + 4  # 4 spaces
    if full_line_width > terminal_width:
        return (terminal_width - size_width * 3 - 4) // 2


def compare(args: object, config: callable) -> None:
    config.convert_f = convert_human_readable if args.human_readable else convert_dummy

    old_stats = parse_file(args.old_version, config)
    new_stats = parse_file(args.new_version, config)

    all_files = sorted(set(list(old_stats.keys()) + list(new_stats.keys())))

    config.file_width = calc_file_width(config.size_width, all_files)

    print_line("Old file", "Size", "Diff", "New file", "Size", config)

    for f in all_files:
        l_file, l_size = get_stat(old_stats, f, config.default_no_value_str)
        r_file, r_size = get_stat(new_stats, f, config.default_no_value_str)

        size_diff = r_size - l_size
        if size_diff == 0:
            if not args.all:
                continue

        if size_diff > l_size * 0.5:
            color = bcolors.WARNING if l_size == 0 else (bcolors.FAIL + bcolors.BOLD)
        elif 0 - size_diff > l_size * 0.5:
            color = bcolors.OKGREEN
        else:
            color = bcolors.ENDC

        if l_file == config.default_no_value_str:
            l_size = config.default_no_value_str
        if r_file == config.default_no_value_str:
            r_size = config.default_no_value_str

        print_line(l_file, l_size, size_diff, r_size, r_file, config, color)


def to_sortable(string: str) -> int:
    v = ["0", "0", "0"]

    for i, elm in enumerate(regex.numeric_only.sub("", string).split(".")):
        v[i] = elm

    return int("".join([elm.rjust(10, "0") for elm in v]))


def gather_files() -> list:
    return sorted(
        [regex.file_extension.sub("", path.basename(f)) for f in glob(config.logs_dir + "*" + config.extension)],
        key=to_sortable,
    )


# Emulate 'ls'-like output in columns
def calc_print_dimensions(strings: str) -> Tuple[int, list]:
    spacing = 3
    try:
        terminal_width = get_terminal_size().columns
    except OSError:
        terminal_width = 1

    list_len = len(strings)
    n_lines = list_len
    n_cols = 1
    widest = [max(len(s) for s in strings) + spacing]
    new_widest = widest

    while sum(new_widest) <= terminal_width:
        n_cols += 1
        n_lines = ceil(list_len / n_cols)

        widest = new_widest
        new_widest = [
            (max(len(s) for s in c) if c else 0) + spacing
            for c in [strings[offset * n_lines : (offset + 1) * n_lines] for offset in range(n_cols)]
        ]

    n_cols = 1 if n_cols <= 2 else n_cols - 1

    # make as wide as possible without going over
    return ceil(list_len / n_cols), n_cols, widest


def list_versions(args: object, config: callable) -> None:
    f_list = gather_files()

    n_lines, n_cols, widest = calc_print_dimensions(f_list)

    if n_cols == 1:  # speed optimization
        return long_list_versions(args, config, f_list)

    for line in range(n_lines):
        print("".join([f.ljust(widest[col]) for col, f in enumerate(f_list[line::n_lines])]))


def long_list_versions(args: object, config: callable, f_list: list = None) -> None:
    print(*(f_list if f_list else gather_files()), sep="\n")


def collect_files(root_dir: str) -> str:
    files_stats = (
        getoutput(
            f"cd '{root_dir}' && du --all --bytes . | while IFS='\t' read -r size line; do printf '%s\\t%s' \"$size\" \"${{line#.}}\"; test -d \"$line\" && printf '/'; echo; done | sort -k2"
        )
        + "\n"
    )
    if args.type == "gpl":
        return files_stats

    f_squashfs = getoutput(f"find '{path.dirname(root_dir)}' -type f -name 'root.squashfs' | head -n1")
    f_kernel = getoutput(f"find '{path.dirname(f_squashfs)}' -maxdepth 1 -type f -name '*.bin' | head -n1")

    return (
        f"{path.getsize(f_kernel)}\t{path.basename(f_kernel)}\n"
        + f"{path.getsize(f_squashfs)}\t{path.basename(f_squashfs)}\n"
        + files_stats
    )


def collect_ipk_files(root_dir: str):
    return getoutput(f"find {root_dir} -name '*.ipk' -exec du --all --bytes {{}} \; | perl -p -e 's/^(\d+\s+).*\//\\1/'")


def save(args: object, config: callable) -> None:
    root_dir = quote(args.info_path)

    if path.isfile(root_dir):
        exit_w_err(f"{args.root_info_pathdir}: not a directory")
    elif not path.isdir(root_dir):
        exit_w_err(f"{args.root_info_pathdir}: no such file or directory")

    files_stats = collect_ipk_files(root_dir) if args.type == "ipk" else collect_files(root_dir)

    output_file = config.logs_dir + config.fw_version + config.extension
    with open(output_file, "w") as file:
        file.write(files_stats)

    print(f"Info about {args.type.upper()} files written to {output_file}")


def parse_args() -> object:
    parser = argparse.ArgumentParser(description="Show / collect information about files' differences between FW versions")

    parser.add_argument(
        "-t",
        "--type",
        choices=["fw", "gpl", "ipk"],
        help="Type of file lists to perform actions on",
        default="fw",
    )
    parser.add_argument("--gpl", action="store_true", help="Shorthand for --type=gpl")

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
    parser_compare.add_argument("-a", "--all", action="store_true", help="Don't skip files that are the same")
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

    parser_list = subparsers.add_parser("list", aliases=["l"], help="List FW versions available for comparison")
    parser_list.set_defaults(func=list_versions)

    parser_long_list = subparsers.add_parser(
        "long-list",
        aliases=["ll"],
        help="Same as 'list', but use long format",
    )
    parser_long_list.set_defaults(func=long_list_versions)

    parser_save = subparsers.add_parser("save", aliases=["s"], help="Save information about the FW file system")
    parser_save.add_argument(
        "info_path",
        help="A root directory of device files (ROM), like build_dir/target-*/root-*",
    )
    parser_save.set_defaults(func=save)

    return parser.parse_args()


def exec_and_print_or_err(cmd: str) -> None:
    code, out = getstatusoutput(cmd)
    print(out)

    if code != 0:
        print(f"\nCommand '{cmd}' failed with code {code}")
        print(f"Use 'force-update' to delete {path.relpath(basedir)} before updating (you'll lose all your unpushed changes there!)")
        exit(2)


def determine_base_dir(initial: str) -> str:
    if path.exists(initial):
        return initial

    basedir = f"{getcwd()}/scripts/rom-diff"
    if path.exists(basedir):
        return basedir

    basedir = f"{path.dirname(__file__)}/rom-diff"
    if path.exists(basedir):
        return basedir

    return ""


args = parse_args()

if args.gpl:
    args.type = "gpl"

if args.type == "fw":
    config.extension = ".files"
elif args.type == "gpl":
    config.extension = ".gpl_files"
elif args.type == "ipk":
    config.extension = ".ipk_files"

regex.file_extension = re_compile("\\" + config.extension + "$")

basedir = determine_base_dir("")

if args.target == "force-update":
    rmtree(basedir)
    args.target = "update"
if args.target == "update":
    for cmd in [
        f"git submodule update --init {basedir}",
        f"git -C '{basedir}' checkout $(git config --file=.gitmodules submodule.\"$(git submodule | awk '{{print $2}}')\".branch)",
        f"git -C '{basedir}' pull",
    ]:
        exec_and_print_or_err(cmd)
        basedir = determine_base_dir(basedir)
    exit(0)

config.logs_dir = path.normpath(f"{basedir}/{args.target}") + "/"
makedirs(config.logs_dir, exist_ok=True)

config.fw_version = getoutput(f"{path.dirname(basedir)}/get_tlt_version.sh --short")

args.func(args, config)
