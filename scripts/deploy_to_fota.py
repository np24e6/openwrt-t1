#!/usr/bin/env python3

import re
from argparse import ArgumentParser
from glob import glob
from hashlib import sha256
from logging import error
from os import getenv, path

import boto3
from botocore.exceptions import ClientError
from mysql import connector

parser = ArgumentParser()

# If any failures occur, all of the database changes that
#   were done by this script are rolled back. Failures
#   are counted in this dictionary and printed at the end
#   of the script.
failure_count = {
    "Parse": 0,
    "AWS": 0,
    "DB": 0
}

debug = False
dry_run = False


def log(*args):
    """
    If debug is enabled, prints all arguments

    Parameters
    ----------
    *args : Any
        Any number of arguments to print
    """

    if debug:
        print(*args)


def error_and_exit(help_method, message=""):
    """
    Prints a message, then calls the help method and exits with 1.

    Parameters
    ----------
    help_method : function
        Function to call after printing an error message, usually used to provide help about the program
    message : str, optional
        An error message
    """

    print("Error: " + message)
    if callable(help_method):
        help_method()
    exit(1)


def getenv_or_error(env_variable):
    """
    Retrieves specified environment variable.
    If that environment variable is not found or empty, prints error and raises 'EnvironmentError'.
    Does not raise 'EnvironmentError' if 'dry_run' is enabled.

    Parameters
    ----------
    env_variable : str
        Name of the environment variable

    Returns
    -------
    string
        Value of the environment variable

    Raises
    ------
    EnvironmentError
        If value of 'env_variable' is not found or is empty and 'dry_run' is not enabled.
    """

    value = getenv(env_variable)
    if value:
        return value

    print(f"Failed to get '{env_variable}' from the environment!", end='')

    if dry_run:
        log(" Will continue regardless because it's a dry run")
    else:
        print('')
        raise EnvironmentError


class MySQL:
    """
    A class to insert information about FW files in AWS into FOTA database.
    If optional string type attributes are not provided, they are read from environment variables.

    Attributes
    ----------
    prefix_env : str
        Prefix for environment variable names that hold MySQL information, e.g. 'DEMO_RUT_FOTA_' for demo FOTA.
    fw_table : str
        Name of the table that holds information about suggested FW versions.
    files_table : str
        Name of the table that holds information about FW files.
    user : str, optional
        Username to login to FOTA database
    password : str, optional
        Password to login to FOTA database
    host : str, optional
        FOTA database server hostname
    database : str, optional
        FOTA database name
    port : int, optional
        FOTA database server port (default 3306)
    """

    prefix_mysql = "MYSQL_"

    """
    Compilation targets mapped to device names in FOTA.
    If target matches the device name exactly (e.g. "RUT2"), then specifying
      it here is optional.
    """
    device_map = {
        # 'RUT2': ('RUT2',),
        'RUT2M': ('RUT200', 'RUT241', 'RUT260'),
        'RUT30X': ('RUT300',),
        'RUT36X': ('RUT360',),
        'RUT9M': ('RUT901', 'RUT906', 'RUT951', 'RUT956'),
        'TRB2M': ('TRB246', 'TRB256')
    }

    def __init__(self, prefix_env, fw_table, files_table, user=None, password=None, host=None, database=None, port=3306):
        # Enforce all parameters to be either passed in or read from the environment
        if not (user and password and host and database):
            # Construct environment variable names and get their values
            user = getenv_or_error(prefix_env + self.prefix_mysql + 'USER')
            password = getenv_or_error(prefix_env + self.prefix_mysql + 'PASSWORD')
            host = getenv_or_error(prefix_env + self.prefix_mysql + 'SERVER')
            database = getenv_or_error(prefix_env + self.prefix_mysql + 'DATABASE')

        env_port = getenv(prefix_env + self.prefix_mysql + 'PORT')
        if env_port:
            port = env_port

        self.fw_table = fw_table
        self.files_table = files_table

        try:
            self.connection = connector.connect(
                host=host,
                user=user,
                password=password,
                database=database,
                port=port
            )
            self.cursor = self.connection.cursor()
        except:
            if dry_run:
                log("Failed to connect to database. Will continue regardless because it's a dry run")

    def get_router_ids(self, target, version, client_number=0):
        """
        Retrieves a list of database IDs for a compilation target whose version is in range between min_fw and max_fw.

        Parameters
        ----------
        target : str
            Compilation target - one of the keys in MySQL.device_map
        version : str
            FOTA-database-compatible version string, e.g. 70020001
        client_number : int
            Client code of the FW, default 0

        Returns
        -------
        list
            A list of device IDs
        """

        devices = "', '".join(self.device_map.get(target.upper(), (target.upper(), )))
        # 4294967295 - MySQL maximum unsigned INT value
        query = f"SELECT id FROM {self.fw_table} WHERE device_name IN ('{devices}') AND \
                        IFNULL(client_number,0) = '{client_number}' AND ( \
                        (max_fw is NULL AND IFNULL(min_fw,0) <= '{version}') OR \
                        (min_fw is NULL AND IFNULL(max_fw,4294967295) >= '{version}')); \
        "
        log(f"get_router_ids() query: {query}")
        self.cursor.execute(query)
        return [i[0] for i in self.cursor.fetchall()]

    def insert_fw_file(self, filename, file_path, file_size, description=None):
        """
        Inserts information about a new FW file into FW files table.

        Parameters
        ----------
        filename : str
            Basename of the new FW file
        file_path : str
            Directory name where the 'filename' file is located
        file_size : int
            Size of the 'filename' file
        description : str, optional
            Misc information about the file

        Returns
        -------
        int
            ID of the newly created record
        """

        # remove '_WEBUI' and '.bin', don't remove '_WEBUI_FAKE'
        name = re.sub(r'_WEBUI\.bin$', '.bin', filename)
        name = re.sub(r'_WEBUI_FAKE.bin', '_WEBUI_FAKE', name)
        name = re.sub(r'\.bin$', '', name)

        description_q = " '" + description + "'," if description else ''
        query = f"INSERT INTO {self.files_table} (name, {'description,' if description else ''} path, files_type_id, size) \
                    VALUES ('{name}',{description_q} '{file_path}', '1', '{file_size}');"
        log(f"insert_fw_file() query: {query}")
        self.cursor.execute(query)
        self.cursor.execute("SELECT LAST_INSERT_ID();")
        return self.cursor.fetchone()[0]

    def set_fw_file_index_for_devices(self, new_fw_index, device, fw_version, client_code=0):
        """
        Sets a new firmware file ID in suggested FWs table for every entry of the device.

        Parameters
        ----------
        new_fw_index : int
            Index of the new FW file in FW files table
        device : str
            Compilation target - one of the keys in MySQL.device_map
        fw_version : str
            FOTA-database-compatible version string, e.g. 70020001
        client_code : int
            Client code of the FW, default 0
        """

        log(f"new_fw_index={new_fw_index}, device={device}, client_code={client_code}, fw_version={fw_version}")

        if not (new_fw_index and device and fw_version):
            raise Exception("Invalid arguments for set_fw_file_index_for_devices()")

        for an_id in self.get_router_ids(device, fw_version, client_code):
            query = f"UPDATE {self.fw_table} SET file_id = '{new_fw_index}' WHERE (id = '{an_id}');"
            log(f"set_fw_file_index_for_devices() query: {query}")
            self.cursor.execute(query)


class AWS:
    """
    A class to upload files to AWS.

    Attributes
    ----------
    prefix_env : str
        Prefix for environment variable names that hold AWS credentials, e.g. 'DEMO_RUT_FOTA_' for demo FOTA.
    """

    def __init__(self, prefix_env):
        # Get values of environment variables
        self.s3_bucket = getenv_or_error(prefix_env + "S3_BUCKET")
        log(f"s3_bucket={self.s3_bucket}")

        self.s3_client = boto3.client('s3')

    def upload(self, subdir, file_path, file_name, dry_run=False):
        """
        Uploads a file to an S3 bucket

        Parameters
        ----------
        subdir : str
            Subdirectory for the file is AWS
        file_path : str
            Full path to the file to upload, excluding file name
        file_name : str
            File name of the file to upload

        Returns
        -------
        str / None
            path in AWS if file was uploaded, else None
        """

        if not (file_path and file_name and subdir):
            return None

        aws_path = path.normpath(f'{subdir}/{file_name}')

        # Upload the file and print URL
        try:
            log(f"Uploading to {self.s3_bucket}/{aws_path}")
            if not dry_run:
                self.s3_client.upload_file(file_path + file_name, self.s3_bucket, aws_path)
                print(f"https://firmware.teltonika-networks.com/{aws_path}")
        except ClientError as e:
            error(e)
            return None

        return aws_path


def construct_version_db(version_str):
    """
    Creates a FOTA-database-compatible version string, e.g. 7.2.1 -> 70020001.

    Sets missing version digits to 0, e.g. 7.2 -> 70020000.

    Ignores anything beyond hotfix value, e.g. 7.2.1.2 -> 70020001 = 7.2.1.

    Parameters
    ----------
    version_str : str
        A normal version string without preceding '0', e.g. 7.2

    Returns
    -------
    str
        FOTA database compatible version string, e.g. 70020001
    """

    if version_str is None:
        version_str = ''

    digit_count = 3
    digits = ['0'] * digit_count

    for i, digit in enumerate(version_str.split(".")):
        if i > digit_count - 1:
            break
        digits[i] = digit

    return digits[0].zfill(1) + digits[1].zfill(3) + digits[2].zfill(4)


def parse_client_and_version(fw_name):
    """
    Extracts version numbers and client code, skipping same FW version iteration count:
    - RUT30X_T_R72_00.07.01.1249_002_WEBUI.bin -> 0, 07.01.1249
    - RUT36X_R_01.07.02.1_WEBUI.bin -> 1, 07.02.1

    Then removes preceding '0' digits from version numbers, like so: 07.02.1 -> 7.2.1

    Parameters
    ----------
    fw_name : str
        String to parse for the version information

    Returns
    -------
    tuple
        A tuple containing client code and version string without any preceding zeros. If version was not found, tuple members are -1 and None.
    """

    if fw_name is None:
        return (-1, None)

    # RUT36X_R_00.07.02.1_WEBUI.bin -> 07.02.1
    match = re.search("_(GPL_)?([0-9]{2})\.([0-9.]+)(_[0-9]+)?(?(1)\.tar\.gz|_((WEBUI|(?:UBOOT.+)?MASTER_STENDUI|)\.bin|Packages\.tar\.gz))", fw_name.split("/")[-1])
    if match is None:
        return (-1, None)

    version = match.group(3)
    log(f"match={match.group(0)}, code={match.group(2)}, version={version}")
    #                                                               07.02.1 -> 7.2.1
    return (-1, None) if version is None else (int(match.group(2)), re.sub(r"0*([1-9]*)([0-9])", "\g<1>\g<2>", version))


def split_path(file):
    """
    Similar to os.path.split(), but preserves trailing slash ('/') in directory

    Parameters
    ----------
    file : str
        Path and name of the file

    Returns
    -------
    tuple
        A (file path, file name) tuple
    """

    if file is None:
        return None, None

    idx = file.rfind('/') + 1
    return file[:idx], file[idx:]


def upload_to_AWS(args, fw_files, prefix_env, dir_prefix="", dir_postfix="", fw_table=None, files_table=None):
    try:
        # We don't need these values directly, but we need to check them because they're later used by AWS
        commit_sha = getenv_or_error("CI_COMMIT_SHA")
        getenv_or_error("AWS_ACCESS_KEY_ID")
        getenv_or_error("AWS_SECRET_ACCESS_KEY")

        aws = AWS(prefix_env)
        mysql = MySQL(prefix_env, fw_table, files_table) if fw_table and files_table else None
    except EnvironmentError:
        error_and_exit(parser.print_help, "Not found required variables in the environment")

    if dir_prefix:
        dir_prefix += "/"
    if dir_postfix:
        dir_postfix = "/" + dir_postfix

    print()
    for file_path, fw_file in fw_files[:]:
        log(f"file_path={file_path}, fw_file={fw_file}")

        client_code, version = parse_client_and_version(fw_file)
        if not version:
            print(f"Failed to parse version of '{fw_file}'")
            failure_count["Parse"] += 1
            fw_files.remove((file_path, fw_file))
            continue

        if re.search("MASTER_STENDUI\.bin$", fw_file):
            postfix = dir_postfix + "/" + sha256(f"{version}|{dir_postfix}|{commit_sha}".encode('utf-8')).hexdigest()
        else:
            postfix = dir_postfix

        aws_path = aws.upload(f"{dir_prefix}{version}{postfix}", file_path, fw_file, args.dry_run)
        if aws_path is None:
            print(f"Failed to upload '{fw_file}'")
            failure_count["AWS"] += 1
            fw_files.remove((file_path, fw_file))
            continue

        if mysql is None:
            continue

        try:
            description = (f"{client_code} client " if client_code > 0 else "") + \
                ("release " if len(version.split('.')) < 3 else "hotfix ") + version

            fw_file_id = mysql.insert_fw_file(fw_file, aws_path, path.getsize(file_path + fw_file), description)
            log(f"fw_file_id={fw_file_id}")
            mysql.set_fw_file_index_for_devices(fw_file_id, args.target, construct_version_db(version), client_code)
        except BaseException as err:
            print(f"Unexpected {err=}")
            failure_count["DB"] += 1
            fw_files.remove((file_path, fw_file))
            continue

    log("Successfully uploaded FWs: ")
    log([_dir + name for _dir, name in fw_files])

    for type in failure_count:
        if failure_count[type] > 0:
            print(f"{type} failed {failure_count[type]} time{'' if failure_count[type] == 1 else 's'}")

    success = sum(failure_count.values()) == 0

    if mysql is not None:
        if success and not args.dry_run:
            mysql.connection.commit()
        else:
            mysql.connection.rollback()

    return not success


def find_by_glob(glob_pattern, recursive=True):
    return [split_path(f) for f in glob(glob_pattern, recursive=recursive)]


def main():
    parser.add_argument("-f", "--dry-run",
                        help="Don't make any actual changes to the DB or AWS. Forces debug output", action="store_true")
    parser.add_argument("-t", "--debug", help="Print some more verbose logs", action="store_true")
    parser.add_argument("-d", "--demo", help="Upload to demo FOTA", action="store_true")
    parser.add_argument("-p", "--production", help="Upload to production FOTA", action="store_true")
    parser.add_argument("-l", "--public", help="Upload to public AWS storage", action="store_true")
    parser.add_argument("target", help="Device target")
    args = parser.parse_args()

    global debug
    global dry_run
    debug = args.debug or args.dry_run
    dry_run = args.dry_run

    args.target = args.target.upper()

    scripts_dir = path.dirname(path.realpath(__file__))
    log(f"scripts_dir={scripts_dir}")

    # Get list of tuples (type, path, name) of all FW files in '<scripts dir>/../bin/'
    fw_files = find_by_glob(f'{scripts_dir}/../bin/**/{args.target}*[0-9]_WEBUI.bin')

    if args.public:
        fw_files.extend(find_by_glob(f'{scripts_dir}/../bin/**/{args.target}*[0-9]_MASTER_STENDUI.bin'))
        fw_files.extend(find_by_glob(f'{scripts_dir}/../{args.target}*_GPL_*[0-9].tar.gz'))
        fw_files.extend(find_by_glob(f'{scripts_dir}/../bin/**/zipped_packages/{args.target}*[0-9]_Packages.tar.gz'))

        return upload_to_AWS(args, fw_files, "FIRMWARE_CDN_", "", args.target)
    elif args.demo or args.production:
        env_prefix = ("DEMO" if args.demo else "PRODUCTION") + "_RUT_FOTA_"

        return upload_to_AWS(args, fw_files, env_prefix, getenv(env_prefix + "BUCKET_PATH"), "", "suggested_firmwares", "files")
    else:
        error_and_exit("no FOTA type specified")


if __name__ == "__main__":
    exit(main())
