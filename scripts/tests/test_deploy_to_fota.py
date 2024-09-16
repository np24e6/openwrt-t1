#!/usr/bin/env python3

import inspect
import os
import sys

sys.path.append(os.path.abspath(os.path.dirname(__file__) + "/.."))

from deploy_to_fota import *

results = {
    'failed': 0,
    'passed': 0
}


def check(expected, actual, results):
    if expected == actual:
        results['passed'] += 1
        return False

    print(f"Fail[{inspect.stack()[1][3]}]: expected {expected}, got {actual}")
    results['failed'] += 1


def test_getenv_or_raise_error(results):
    data = {
        'TEST_VARIABLE_55': 'test_value',
        'TEST_NUMBER': '5555',
        'TEST_TRUE': 'true',
        'TEST_NOT_TRUE': 'false',
        'TEST_None': 'None',
        'TEST_EMPTY': '',
        'TEST_SPACE': ' ',
        'TEST_QUOTE': '"'
    }

    for arg in data:
        try:
            os.environ[arg] = data[arg]
            check(data[arg], getenv_or_raise_error(arg), results)
        except:
            if data[arg] == '':
                results['passed'] += 1
            else:
                results['failed'] += 1


def test_aws_upload(results):
    data = [
        # (TEST_ENV_BUCKET_PATH, TEST_ENV_S3_BUCKET, subdir, file_path, file_name, ret_value)
        ('test/folder', 'bucket1', '7.2.1', 'trb5_open/bin/targets/sdxprairie/generic/tltFws/',
         'TRB500_T_F6304_00.07.02.67_000_WEBUI.bin', 'test/folder/7.2.1/TRB500_T_F6304_00.07.02.67_000_WEBUI.bin'),

        ('some-other/bucket/path', '101_buckets', '8.0', './',
         'RUT30X_T_DEV_00.07.02.310_WEBUI.bin', 'some-other/bucket/path/8.0/RUT30X_T_DEV_00.07.02.310_WEBUI.bin'),

        ('test02/a_firmware', 'test999-rut-fota', '7.3', '/builds/teltonika/rutx_open/scripts/../bin/targets/ipq40xx/generic/tltFws/',
         'RUTX_R_00.07.03_WEBUI.bin', 'test02/a_firmware/7.3/RUTX_R_00.07.03_WEBUI.bin'),

        ('test/fw', 'test5-fota', '7.3', '/builds/teltonika/rutx_open/scripts/../bin/targets/ipq40xx/generic/tltFws/',
         None, None),

        ('test/fw2', 'test5-fota', '7.3', '/builds/teltonika/rutx_open/scripts/../bin/targets/ipq40xx/generic/tltFws/',
         '', None),

        ('test00/a_firmware', 'test999-rut-fota', None, '/builds/teltonika/rutx_open/scripts/../bin/targets/ipq40xx/generic/tltFws/',
         'RUTX_R_00.07.03_WEBUI.bin', None),

        ('test01/a_firmware', 'test88-rut-fota', '', '/builds/teltonika/rutx_open/scripts/../bin/targets/ipq40xx/generic/tltFws/',
         'RUTX_R_00.07.03_WEBUI.bin', None),

        ('test/fw3', 'test6-fota', '7.3', None,
         'RUTX_R_00.07.03_WEBUI.bin', None),

        ('test5/fw', 'test5-fota', '7.3', '',
         'RUTX_R_00.07.03_WEBUI.bin', None),
    ]

    for arg1, arg2, subdir, file_path, file_name, ret_value in data:
        os.environ['TEST_ENV_BUCKET_PATH'] = arg1
        os.environ['TEST_ENV_S3_BUCKET'] = arg2

        try:
            aws = AWS("TEST_ENV_")
            check(ret_value, aws.upload(subdir, file_path, file_name, dry_run=True), results)
        except BaseException as err:
            print(f"Error {err=}")
            results['failed'] += 1


def test_construct_version_db(results):
    data = {
        '7.2.1': '70020001',
        '6.8.50': '60080050',
        '8.88.8888': '80888888',
        '7.3': '70030000',
        '8.0': '80000000',
        '8.1.55.1': '80010055',
        '8.88.888.8888.88888': '80880888',
        '7': '70000000',
        '': '00000000',
        None: '00000000'
    }

    for arg in data:
        try:
            check(data[arg], construct_version_db(arg), results)
        except BaseException as err:
            print(f"Error {err=}")
            results['failed'] += 1


def test_parse_client_and_version(results):
    data = {
        'RUT30X_T_R72_00.07.01.1249_002_WEBUI.bin': (0, '7.1.1249'),
        'somePath/RUT30X_T_R72_00.07.01.1249_002_WEBUI.bin': (0, '7.1.1249'),
        'RUT36X_R_01.07.02.1_WEBUI.bin': (1, '7.2.1'),
        'test_file.bin': (None, None),
        'test_string': (None, None),
        'RUTX_R_00.07.00_WEBUI.bin': (0, '7.0'),
        'RUT9_R72_00.07.000000_WEBUI.bin': (0, '7.0'),
        'RUT9_R72_99.07.000000_WEBUI.bin': (99, '7.0'),
        None: (None, None),
        '': (None, None),
        './': (None, None),
        '.': (None, None)
    }

    for arg in data:
        try:
            check(data[arg], parse_client_and_version(arg), results)
        except BaseException as err:
            print(f"Error {err=}")
            results['failed'] += 1


def test_split_path(results):
    data = {
        '/home/margiris/repos/rutx_open/bin/targets/ipq40xx/generic/tltFws/RUTX_T_DEV_00.07.02.1332_WEBUI.bin':
        ('/home/margiris/repos/rutx_open/bin/targets/ipq40xx/generic/tltFws/', 'RUTX_T_DEV_00.07.02.1332_WEBUI.bin'),

        'trb5_open/bin/targets/sdxprairie/generic/tltFws/TRB500_T_F6304_00.07.02.67_000_WEBUI.bin':
        ('trb5_open/bin/targets/sdxprairie/generic/tltFws/', 'TRB500_T_F6304_00.07.02.67_000_WEBUI.bin'),

        '../bin/targets/sdxprairie/generic/tltFws/TRB500_T_F6304_00.07.02.67_WEBUI.bin':
        ('../bin/targets/sdxprairie/generic/tltFws/', 'TRB500_T_F6304_00.07.02.67_WEBUI.bin'),

        'RUT30X_T_DEV_00.07.02.310_WEBUI.bin':
        ('', 'RUT30X_T_DEV_00.07.02.310_WEBUI.bin'),

        '../bin/targets/ipq40xx/generic/tltFws/':
        ('../bin/targets/ipq40xx/generic/tltFws/', ''),

        '.':
        ('', '.'),

        './':
        ('./', ''),

        '..':
        ('', '..'),

        '../':
        ('../', ''),

        '':
        ('', ''),

        None:
        (None, None)
    }

    for arg in data:
        check(data[arg], split_path(arg), results)


if __name__ == "__main__":
    tests = [
        test_getenv_or_raise_error,
        test_aws_upload,
        test_construct_version_db,
        test_parse_client_and_version,
        test_split_path
    ]

    for test in tests:
        print(f"\nRunning tests for {test.__name__}...")
        test(results)

    total = results['failed'] + results['passed']
    print("\nTest results:")
    print(f"\tTested: {total:3}")
    print(f"\tPassed: {results['passed']:3}")
    print(f"\tFailed: {results['failed']:3}")

    exit(results['failed'])
