#!/usr/bin/env python3

from collections import defaultdict
from os import listdir, path, walk
import re
import sys

# Example of the data structure:
# stats = {
#     "7.5.1": {
#         "RUTX": 5,
#         "TAP100": 1
#     },
#     "7.6": {
#         "RUT9": 43,
#         "RUTX": 12
#     }
# }

operation_GET = "REST.GET.OBJECT"
operation_PUT = "REST.PUT.OBJECT"

http_status_OK = 200

log_pattern = re.compile(r" ?(\")?(\[)?((?(1)[^\"]*|(?(2)[^\]]*|[^ ]*)))[\"\] ]")
version_pattern = re.compile(r"(?<=\/)[0-9](?:\.[0-9]+)*(?=\/)")
device_pattern = re.compile(r"(?<=\/)[A-Za-z0-9]+(?=_)")


class LogEntry:
    def __init__(self, line):
        props_list = [group[2] for group in log_pattern.findall(line)]
        # print(props_list)
        # self.bucket_owner_id = props_list[0]
        self.bucket_name = props_list[1]
        self.timestamp = props_list[2]
        self.ip = props_list[3]
        self.requester_id = props_list[4]
        # self.request_id = props_list[5]
        self.operation = props_list[6]
        self.object_fullpath = props_list[7]
        # self.full_request_text = props_list[8]
        self.http_status = int(props_list[9])
        # self.error_code = props_list[10]
        # self.bytes_sent = props_list[11]
        # self.object_size = props_list[12]
        # self.total_time = props_list[13]
        # self.turn-around_time = props_list[14]
        # self.referer_header = props_list[15]
        # self.useragent = props_list[16]
        # self.version_id = props_list[17]
        # self.host_id = props_list[18]
        # self.signature_version = props_list[19]
        # self.cipher_suite = props_list[20]
        # self.auth_type = props_list[21]
        # self.host_header = props_list[22]
        # self.tls_version = props_list[23]
        # self.access_point_ARN = props_list[24]
        # self.is_acl_required = props_list[25]

    def __repr__(self):
        return f"{self.timestamp}\tip: {self.ip}\toperation: {self.operation}\tobject_fullpath: {self.object_fullpath}\thttp_status: {self.http_status}\n"


def help():
    print("""Usage: parse_AWS_logs.py [-h] [FILE|DIRECTORY]...

Count AWS download statistics using all FILEs and/or files in all DIRECTORies.

Arguments
    FILE        An AWS log file
    DIRECTORY   A directory containing AWS log files (will be searched recursively)

Options
    -h | --help Print this help and exit
""")


def parse(string, pattern):
    result = None

    re_match = pattern.search(string)
    if re_match:
        result = re_match.group()

    return result


def default_version_value():
    return defaultdict(int)


def get_files(filepaths):
    # use a set to avoid reading same file twice because of duplicate file entries
    files = set()

    for arg in filepaths:
        if path.isdir(arg):
            for dirpath, _, filenames in walk(arg):
                files |= {path.join(dirpath, name) for name in filenames}
        elif path.isfile(arg):
            files.add(arg)

    return files


def read_logs(files):
    logs = []
    for file in files:
        with open(file) as log_file:
            logs += [LogEntry(row) for row in log_file]

    return logs


def count(logs):
    # stats is created with a default value for non-existent keys which are automatically set to 0 on access
    stats = defaultdict(default_version_value)

    for log in logs:
        version = parse(log.object_fullpath, version_pattern)
        device = parse(log.object_fullpath, device_pattern)

        if not version or not device:
            print(f"Failed to parse {log.object_fullpath}")
            continue

        stats[version][device] += 1

    return stats


def print_fmt(version, device, count, head=None, tail=None):
    head = str(head) + "\n" if head else ""
    tail = "\n" + str(tail) if tail else ""
    print(f"{head}{version: <10}{device : <8}{count: >10}{tail}")


def main():
    if len(sys.argv) <= 1 or "-h" in sys.argv or "--help" in sys.argv:
        help()
        exit(len(sys.argv) <= 1)

    print("Listing log files...")
    files = get_files(sys.argv[1:])

    print("Reading logs...")
    logs = read_logs(files)

    print("Counting stats...")
    stats = count(
        [
            l
            for l in logs
            if l.http_status == http_status_OK and l.operation == operation_GET
        ]
    )

    print_fmt("Version", "Device", "Count")

    total = 0
    for version in dict(sorted(stats.items())):
        curr_total = sum(stats[version].values())
        total += curr_total
        print_fmt(version, "All", curr_total, head="_" * 28, tail=" ")

        for device in dict(sorted(stats[version].items())):
            print_fmt("", device, stats[version][device])

    print_fmt("Total", "", total, head="_" * 28)


if __name__ == "__main__":
    exit(main())
