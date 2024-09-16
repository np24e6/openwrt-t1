from pathlib import Path
from timeit import default_timer as timer
import json
import copy

import tbot
from tbot import machine, testcase
from tbot.tc import shell

class RutOSVersion:
	def __init__(self, version, dev):
		arr = version.split('_')

		index = 3
		if len(arr) == 3:
			index = 2

		self.dev = dev
		self.name = arr[0]
		self.type = arr[1]

		if index == 3:
			self.type = self.type + '_' + arr[2]

		ver = arr[index].split('.')

		self.client = ver[0]
		self.major = ver[1]
		self.minor = ver[2]
		self.patch = ''

		if len(ver) >= 4:
			self.patch = ver[3]

	def __str__(self):
		ver = self.name + '_'
		ver = ver + self.type + '_'
		ver = ver + str(self.client) + '.'
		ver = ver + str(self.major) + '.'
		ver = ver + str(self.minor)

		if self.patch:
			ver = ver + '.' + str(self.patch)

		return ver

@testcase
def compare_validation_output(lab: machine.linux.LinuxShell, fields, output) -> None:
	for k, v in fields.items():
		if output[k] != v:
			tbot.log.message(f'Unable to validate firmware image - `{k}` should be {v}.')
			return False

	return True

@testcase
def validate_firmware(lab: machine.linux.LinuxShell, f, dev) -> None:
	tbot.log.message(f'Checking version `{f}`...')
	lab.exec0('/tmp/genfw/genfw.sh', f, dev, '/tmp/genfw/sysupgrade.meta', '/tmp/firmware.bin')
	return json.loads(lab.exec0('/usr/libexec/validate_firmware_image', '/tmp/firmware.bin'))

@testcase
def parse_current_version(lab: machine.linux.LinuxShell) -> None:
	out = lab.exec0('cat', '/etc/version').strip()
	dev = lab.exec0('cat', '/tmp/sysinfo/board_name').strip()

	ver = RutOSVersion(out, dev)

	tbot.log.message(f'Device Name: \t\t{ver.name}\n'
					 f'Firmware Type: \t\t{ver.type}\n'
					 f'Client Number: \t\t{ver.client}\n'
					 f'Major Version: \t\t{ver.major}\n'
					 f'Minor Version: \t\t{ver.minor}\n'
					 f'Patch Version: \t\t{ver.patch}\n'
					 f'Board Name: \t\t{ver.dev}')

	return ver

@testcase
def change_firmware_version(lab: machine.linux.LinuxShell, ver) -> None:
	dev = ver.dev
	f = str(ver)

	if not lab.test('ls', '/etc/version.bak'):
		lab.exec0('mv', '/etc/version', '/etc/version.bak')

	lab.exec0('echo', f, machine.linux.RedirStdout(lab.fsroot / 'etc/version'))

@testcase
def revert_firmware_version(lab: machine.linux.LinuxShell) -> None:
	lab.exec0('mv', '/etc/version.bak', '/etc/version')

@testcase
def upload_fw_gen_files(lab: machine.linux.LinuxShell) -> None:
	lab.exec0('rm', '-rf', '/tmp/genfw')
	lab.exec0('mkdir', '/tmp/genfw')

	shell.copy(
		p1=machine.linux.Path(lab.host, Path(__file__).parent/'genfw.sh'),
		p2=machine.linux.Path(lab, '/tmp/genfw'),
	)

	shell.copy(
		p1=machine.linux.Path(lab.host, Path(__file__).parent/'sysupgrade.meta'),
		p2=machine.linux.Path(lab, '/tmp/genfw'),
	)

@testcase
def remove_fw_gen_files(lab: machine.linux.LinuxShell) -> None:
	lab.exec0('rm', '-rf', '/tmp/fwgen')

@testcase
def check_and_validate_firmware(lab: machine.linux.LinuxShell, vt, current, invalid) ->None:
	# these fields should be set on a different type
	fields_different = {
		'valid': True,
		'allow_backup': False,
		'is_downgrade': True,
	}

	# these fields should be set on the same type
	fields_identical = {
		'valid': True,
		'allow_backup': True,
		'is_downgrade': False,
	}

	dev = current.dev

	for k, v in vt.items():
		# modify firmware type and switch to a new version
		current.type = k
		change_firmware_version(lab, current)

		for j in vt:
			invalid.type = j
			f = str(invalid)
			s = validate_firmware(lab, f, dev)

			# by default, different fields should be applied
			fields = fields_different

			for t in v:
				if t in j:
					fields = fields_identical
					break

			if not compare_validation_output(lab, fields, s):
				revert_firmware_version(lab)
				exit(1)

	revert_firmware_version(lab)

@testcase
def check_wrong_device_name(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)
	dev = 'teltonika,this-is-unsupported-device'
	f = str(info)

	tbot.log.message(f'Checking version `{f}` with `{dev}` device name...')
	lab.exec0('/tmp/genfw/genfw.sh', f, dev, '/tmp/genfw/sysupgrade.meta', '/tmp/firmware.bin')

	s = json.loads(lab.exec0('/usr/libexec/validate_firmware_image', '/tmp/firmware.bin'))

	if s['tests']['fwtool_device_match'] != False:
		tbot.log.message('Unable to validate firmware image - fwtool_device_match should be false.')
		exit(1)

	# return to normal
	dev = info.dev

	tbot.log.message(f'Checking version `{f}` with `{dev}` device name...')
	lab.exec0('/tmp/genfw/genfw.sh', f, dev, '/tmp/genfw/sysupgrade.meta', '/tmp/firmware.bin')

	s = json.loads(lab.exec0('/usr/libexec/validate_firmware_image', '/tmp/firmware.bin'))

	if s['tests']['fwtool_device_match'] != True:
		tbot.log.message('Unable to validate firmware image - fwtool_device_match should be true.')
		exit(1)

@testcase
def check_different_firmware_type(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# this table is parsed as:
	# when coming from T_F1234 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# when coming from T_H123 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# ...
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_F4567': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_H123':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_R96':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_DEV':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R':       [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R_GPL':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'AAFAKE':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_different_client_no(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# since this will test different client numbers, allow_backup should be
	#  always false
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ ],
		'T_F4567': [ ],
		'T_H123':  [ ],
		'T_R96':   [ ],
		'T_DEV':   [ ],
		'R':       [ ],
		'R_GPL':   [ ],
		'AAFAKE':  [ ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.client = '01'
	info_invalid.client = '02'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_newer_major_version(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# this table is parsed as:
	# when coming from T_F1234 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# when coming from T_H123 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# ...
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_F4567': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_H123':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_R96':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_DEV':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R':       [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R_GPL':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'AAFAKE':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.major = '01'
	info_invalid.major = '02'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_older_major_version(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# since this will test different with older major version, allow_backup
	#  should be always false
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ ],
		'T_F4567': [ ],
		'T_H123':  [ ],
		'T_R96':   [ ],
		'T_DEV':   [ ],
		'R':       [ ],
		'R_GPL':   [ ],
		'AAFAKE':  [ ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.major = '02'
	info_invalid.major = '01'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_newer_minor_version(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# this table is parsed as:
	# when coming from T_F1234 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# when coming from T_H123 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# ...
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_F4567': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_H123':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_R96':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_DEV':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R':       [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R_GPL':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'AAFAKE':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.minor = '01'
	info_invalid.minor = '02'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_older_minor_version(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# since this will test different with older minor version, allow_backup
	#  should be always false
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ ],
		'T_F4567': [ ],
		'T_H123':  [ ],
		'T_R96':   [ ],
		'T_DEV':   [ ],
		'R':       [ ],
		'R_GPL':   [ ],
		'AAFAKE':  [ ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.minor = '02'
	info_invalid.minor = '01'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_newer_patch_version(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# this table is parsed as:
	# when coming from T_F1234 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# when coming from T_H123 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# ...
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_F4567': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_H123':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_R96':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_DEV':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R':       [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R_GPL':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'AAFAKE':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.patch = '1'
	info_invalid.patch = '2'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

@testcase
def check_older_patch_version(lab: machine.linux.LinuxShell) -> None:
	info = parse_current_version(lab)

	# we test with all possible firmware types:
	# test: feature, hotfix, release, develop
	# release, release gpl
	# malformed

	# this table is parsed as:
	# when coming from T_F1234 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# when coming from T_H123 to T_F/T_H/T_R/T_DEV/R/R_GPL allow_backup should be true
	# ...
	vt = {
		# <from>   <allow_backup is true on these types>
		'T_F1234': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_F4567': [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_H123':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_R96':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'T_DEV':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R':       [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'R_GPL':   [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ],
		'AAFAKE':  [ 'T_F', 'T_H', 'T_R', 'T_DEV', 'R', 'R_GPL' ]
	}

	info_current = copy.deepcopy(info)
	info_invalid = copy.deepcopy(info)

	info_current.patch = '2'
	info_invalid.patch = '1'

	check_and_validate_firmware(lab, vt, info_current, info_invalid)

def all(lab: machine.linux.LinuxShell) -> None:
	start = timer()
	tbot.log_event.testcase_begin('base-files:fw-version-validation')

	upload_fw_gen_files(lab)

	check_wrong_device_name(lab)

	check_different_firmware_type(lab)

	check_different_client_no(lab)

	check_newer_major_version(lab)
	check_older_major_version(lab)

	check_newer_minor_version(lab)
	check_older_minor_version(lab)

	check_newer_patch_version(lab)
	check_older_patch_version(lab)

	remove_fw_gen_files(lab)

	end = timer()
	tbot.log_event.testcase_end('base-files:fw-version-validation', end - start)
