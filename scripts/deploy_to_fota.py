#!/usr/bin/env python3

import re
from argparse import ArgumentParser
from glob import glob
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
    'Parse': 0,
    'AWS': 0,
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
    Prints a message, then calls the help method if it's callable and exits with code 1.

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


def getenv_or_raise_error(env_variable):
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
        If value of 'env_variable' is not found or empty and 'dry_run' is not enabled.
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
      it here is optional (hence all the commented-out values).
    """
    device_map = {
        # 'OTD1': ('OTD1',),
        # 'RUT2': ('RUT2',),
        'RUT2M': ('RUT200', 'RUT241', 'RUT260'),
        'RUT30': ('RUT300',),
        # 'RUT301': ('RUT301',),
        'RUT36': ('RUT360',),
        # 'RUT361': ('RUT361',),
        # 'RUT9': ('RUT9',),
        'RUT9M': ('RUT901', 'RUT906', 'RUT951', 'RUT956'),
        # 'RUTM': ('RUTM',),
        # 'RUTX': ('RUTX',),
        # 'TAP100': ('TAP100',),
        # 'TCR1': ('TCR1',),
        # 'TRB1': ('TRB1',),
        # 'TRB2': ('TRB2',),
        'TRB2M': ('TRB246', 'TRB256'),
        # 'TRB5': ('TRB5',),
        # 'TSW2': ('TSW2',)
    }

    def __init__(self, prefix_env, fw_table, files_table, user=None, password=None, host=None, database=None, port=3306):
        # Enforce all parameters to be either passed in or read from the environment
        if not (user and password and host and database):
            # Construct environment variable names and get their values
            user = getenv_or_raise_error(prefix_env + self.prefix_mysql + 'USER')
            password = getenv_or_raise_error(prefix_env + self.prefix_mysql + 'PASSWORD')
            host = getenv_or_raise_error(prefix_env + self.prefix_mysql + 'SERVER')
            database = getenv_or_raise_error(prefix_env + self.prefix_mysql + 'DATABASE')

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
        self.bucket_path = getenv_or_raise_error(prefix_env + "BUCKET_PATH")
        self.s3_bucket = getenv_or_raise_error(prefix_env + "S3_BUCKET")

        log(f"bucket_path={self.bucket_path}, s3_bucket={self.s3_bucket}")

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

        # Replace two or more '/' with a single '/'
        aws_path = re.sub('\/{2,}', '/', f'{self.bucket_path}/{subdir}/{file_name}')

        # Upload the file
        try:
            print(f"Uploading {file_path + file_name} to {self.s3_bucket}/{aws_path}")
            if not dry_run:
                self.s3_client.upload_file(file_path + file_name, self.s3_bucket, aws_path)
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
        A tuple containing client code and version string without any preceding zeros. If version was not found, both tuple members are None.
    """

    if fw_name is None:
        return (None, None)

    # RUT36X_R_00.07.02.1_WEBUI.bin -> 07.02.1
    match = re.search("_([0-9]{2})\.([0-9.]+)(_[0-9]+)?(_WEBUI(_FAKE)?)?\.bin", fw_name.split("/")[-1])
    if match is None:
        return (None, None)

    log(f"match={match.group(0)}, code={match.group(1)}, version={match.group(2)}")
    version = match.group(2)
    #                                                                 07.02.1 -> 7.2.1
    return (None, None) if version is None else (int(match.group(1)), re.sub(r"0*([1-9]*)([0-9])", "\g<1>\g<2>", version))


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


def main():
    parser.add_argument("-f", "--dry_run",
                        help="Don't make any actual changes to the DB or AWS. Forces debug output", action="store_true")
    parser.add_argument("-t", "--debug", help="Print some more verbose logs", action="store_true")
    parser.add_argument("-d", "--demo", help="Upload to demo FOTA", action="store_true")
    parser.add_argument("-p", "--production", help="Upload to production FOTA", action="store_true")
    parser.add_argument("target", help="Device target")
    args = parser.parse_args()

    if not (args.demo or args.production):
        error_and_exit("no FOTA type specified")

    global debug
    global dry_run
    debug = args.debug or args.dry_run
    dry_run = args.dry_run

    fw_table = "suggested_firmwares"
    files_table = "files"

    # Construct dynamic string based on FOTA type
    prefix_env = ("PRODUCTION" if args.production else "DEMO") + "_RUT_FOTA_"

    scripts_dir = path.dirname(path.realpath(__file__))
    log(f"scripts_dir={scripts_dir}")

    try:
        # We don't need these values directly, but we need to check them because they're later used by AWS
        getenv_or_raise_error("AWS_ACCESS_KEY_ID")
        getenv_or_raise_error("AWS_SECRET_ACCESS_KEY")

        aws = AWS(prefix_env)
        mysql = MySQL(prefix_env, fw_table, files_table)
    except EnvironmentError:
        error_and_exit(parser.print_help, "Not found required variables in the environment")

    # Get list of tuples (file path, file name) of all FW files in 'scripts_dir/../bin/'
    fw_files = [split_path(fw_file) for fw_file in glob(f'{scripts_dir}/../bin/**/*[0-9]_WEBUI*.bin', recursive=True)]

    for file_path, fw_file in fw_files[:]:
        log(f"file_path={file_path}, fw_file={fw_file}")

        client_code, version = parse_client_and_version(fw_file)
        if version is None:
            print(f"Failed to parse version of '{fw_file}'")
            failure_count["Parse"] += 1
            fw_files.remove((file_path, fw_file))
            continue

        log(f"version={version}")

        # Use FW version as a subdirectory
        aws_path = aws.upload(version, file_path, fw_file, dry_run)
        if aws_path is None:
            print(f"Failed to upload '{fw_file}'")
            failure_count["AWS"] += 1
            fw_files.remove((file_path, fw_file))
            continue

        description = (f"{client_code} client " if client_code > 0 else "") + \
            ("release " if len(version.split('.')) < 3 else "hotfix ") + version

        try:
            fw_file_id = mysql.insert_fw_file(fw_file, aws_path, path.getsize(file_path + fw_file), description)
            log(f"fw_file_id={fw_file_id}")
            mysql.set_fw_file_index_for_devices(fw_file_id, args.target, construct_version_db(version), client_code)
        except BaseException as err:
            print(f"Unexpected {err=}")
            failure_count["DB"] += 1
            fw_files.remove((file_path, fw_file))
            continue

    print("Successfully uploaded FWs: ")
    print([_dir + name for _dir, name in fw_files])

    for type in failure_count:
        if failure_count[type] > 0:
            print(f"{type} failed {failure_count[type]} time(s)")

    success = sum(failure_count.values()) == 0
    if success and not dry_run:
        mysql.connection.commit()
        return 0
    else:
        mysql.connection.rollback()
        #     success (1) and not dry_run (0) -> 0 (won't get here) -> return 0
        #     success (1) and     dry_run (1) -> 1 -> return 0
        # not success (0) and not dry_run (0) -> 0 -> return 1
        # not success (0) and     dry_run (1) -> 0 -> return 1
        return not (success and dry_run)


if __name__ == "__main__":
    exit(main())
