#!/usr/bin/env python3

import re

from argparse import ArgumentParser
from os import path
from sys import exit


def parse_args() -> object:
    parser = ArgumentParser(description="Print information about target architectures and profiles")

    parser.add_argument(
        "-a",
        "--all",
        action="count",
        default=0,
        help="Print not the first one but all profiles. Specifying more than once prints model-specific targets as well (e.g. TRB1401). Does not have any effect when --target is used",
    )
    parser.add_argument(
        "-f",
        "--format",
        choices=["ci", "table"],
        default="table",
        help="Print in YAML format for pipelines (ci) or a plaintext table (default)",
    )
    parser.add_argument(
        "-t",
        "--target",
        help="Print information only for specified target",
    )
    parser.add_argument(
        "-x",
        "--exclude",
        action="append",
        help="Exclude target from list",
    )

    parser.add_argument(
        "targetinfo_file",
        nargs="?",
        default=path.dirname(path.realpath(__file__)) + "/../tmp/.targetinfo",
        help="File to parse the target information from",
    )

    return parser.parse_args()


args = parse_args()

try:
    with open(args.targetinfo_file, "r") as file:
        content = file.read()
except FileNotFoundError as e:
    print(
        f"Target info file is required and default {args.targetinfo_file} was not found. Please specify; or run 'make defconfig' to generate a default one."
    )
    exit(e.errno)
except Exception as e:
    print(f"Error reading the file: {e}")
    exit(e.errno)

arch_pattern = re.compile(r"Target-Arch: (\S+)")
cpu_pattern = re.compile(r"CPU-Type: (\S+)")
profile_pattern = re.compile(r"Target-Profile:\s+DEVICE_teltonika_(\S+)")

unique_combinations = {}

current_arch = None
current_cpu = None

for line in content.splitlines():
    arch_match = arch_pattern.search(line)
    cpu_match = cpu_pattern.search(line)
    profile_match = profile_pattern.search(line)

    if arch_match:
        current_arch = arch_match.group(1)

    if cpu_match:
        current_cpu = cpu_match.group(1)

    if profile_match and current_arch and current_cpu:
        profile = profile_match.group(1)
        target_name = re.sub(r"x{2,}$", "", profile, flags=re.MULTILINE)
        if (args.exclude and current_arch in args.exclude) or (args.all < 2 and len(profile) > 6):  # skip 'trb1411' and alike, as well as excluded targets
            profile_match = None
            continue

        key = (current_arch, current_cpu)
        if key not in unique_combinations or not unique_combinations[key]:
            unique_combinations[key] = list()

        unique_combinations[key].append(target_name)


if args.format == "ci":
    indent = "\n" + " " * 6
    env_var_prefix = "DOCKER_BUILD_ARG_"

    def print_line(arch: str, cpu: str, profile: str) -> None:
        print(
            f"{indent}- {env_var_prefix}ARCH: {arch}{indent}  {env_var_prefix}CPU_TYPE: {cpu}{indent}  {env_var_prefix}SELECT_TARGET: [ {profile} ]"
        )

elif args.format == "table":

    def print_line(arch: str, cpu: str, profile: str) -> None:
        print(f"{arch: <15}{cpu: <30}{profile}")

    if not args.target:
        print_line("Arch", "CPU type", "Targets" if args.all else "First target")

for (arch, cpu), profiles in unique_combinations.items():
    profiles = list(dict.fromkeys(profiles))  # keep only unique items

    if args.target:
        if args.target not in profiles:
            continue
        profiles = [args.target]

    print_line(
        arch,
        cpu,
        ", ".join(profiles) if args.all else profiles[0],
    )
