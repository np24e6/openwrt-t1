#!/usr/bin/env python3

import sys
import os
import subprocess
import json
import re
from os import path
from subprocess import Popen

list_profiles = list()
list_features = list()
list_packages = list()
dump_list = False

def empty_none_value(value):
    if value is None:
        return ''
    
    return value

def is_match(regex, text):
    return re.compile(regex).search(text) is not None

def type_cast(value):
    if isinstance(value, str) and value.isdigit():
        return int(value.strip())
    return value.strip()

def recursive_insert(dct, keys, value):
    if len(keys) == 1:
        if ";" in value:
            value = value.split(';')
            stripped_values = [v.strip() for v in value if v]
           
            if keys[0] in dct and isinstance(dct[keys[0]], list):
                current = dct[keys[0]]
                dct[keys[0]] = list(set(current + stripped_values))
            else:
                dct[keys[0]] = stripped_values
        else:
            dct[keys[0]] = type_cast(value)
    else:
        if keys[0] not in dct or not isinstance(dct[keys[0]], dict):
            dct[keys[0]] = {}
        recursive_insert(dct[keys[0]], keys[1:], value)

def find_pkg_by_name(name):
    for pkg in list_packages:
        if pkg.name == name:
            return pkg

    return None

class Target:
    def __init__(self, name):
        self.name = name
        self.features = []
    
    def target(self):
        data = self.name.split('/')
        return data[0]

    def subtarget(self):
        data = self.name.split('/')

        if len(data) == 1:
            return 'none'

        return data[1]

    def parse(self, file):
        for l in file:
            if is_match('^@@$', l):
                break
            elif is_match('^Target-Features: ', l):
                self.features = set(l[17:-1].split())

class Profile:
    def __init__(self, profile, target):
        self.name = 'Unknown'
        self.target = target
        self.profile = profile
        self.features = []
        self.hw = {}
        self.conf = {}
        self.features = []
        self.cached_packages = {}

    def parse(self, file):
        hw = {}

        for l in file:
            if is_match('^@@$', l):
                break
            elif is_match('^Target-Profile-Name:', l):
                self.name = l[21:-1]
            elif is_match('^Target-Profile-Features: ', l):
                self.features = set(l[25:-1].split())
                self.features.update(self.target.features)
            elif 'Target-Profile-HARDWARE-' in l:
                if len(l.strip()[24:].split(": ")) != 2:
                    continue

                key, value = l.strip()[24:].split(": ")
                key_parts = key.lower().split('-')
                recursive_insert(hw, key_parts, value)
                self.hw = hw

    def prepare_dotconfig(self):
        f = open('.config', 'w')

        f.write(f'CONFIG_TARGET_{self.target.target()}=y\n')
        f.write(f'CONFIG_TARGET_{self.target.target()}_{self.target.subtarget()}=y\n')
        f.write(f'CONFIG_TARGET_{self.target.target()}_{self.target.subtarget()}_{self.profile}=y\n')

        f.close()

        Popen('make defconfig', shell=True, stdout=subprocess.DEVNULL).wait()

    def read_config(self):
        with open('.config', 'r') as config_file:
            for line in config_file:
                line = line.strip()
                if line.endswith("is not set"):
                    self.conf[line.split(" ")[1].split("CONFIG_")[1]] = False
                elif "=" in line:
                    option, value = line.split("=", 1)
                    self.conf[option.split("CONFIG_")[1]] = True if value == 'y' else value
    
    def fetch_features(self):
        self.features = [f for f in list_features if f.name in self.conf]

    def find_package_deps(self, deps):
        packages = []

        for d in deps:
            if d in self.cached_packages:
                packages.append(self.cached_packages[d])
                continue

            if f'PACKAGE_{d}' not in self.conf:
                continue

            pkg = find_pkg_by_name(d)
        
            info = {
                'name': pkg.name,
                'title': pkg.title,
                'maintainer': pkg.maint,
                'description': pkg.desc,
                'detail': pkg.detail,
                'depends': self.find_package_deps(pkg.deps),
            }

            self.cached_packages[pkg.name] = info

            packages.append(info)

        return packages

    def json(self):
        data = {
            'name': self.name,
            'target': {
                'name': self.target.target(),
                'subtarget': self.target.subtarget(),
                'profile': self.profile
            },
            'hardware': self.hw,
            'features': [],
        }

        self.prepare_dotconfig()
        self.read_config()
        self.fetch_features()

        data['features'] = dump_features(self.features)

        return json.dumps(data, indent=2)

def initialize_targets():
    ti = 'tmp/.targetinfo'

    if not path.exists(ti):
        print('Target info does not exist, running defconfig...')
        Popen('make defconfig', shell=True).wait()

    if not path.exists(ti):
        print('Unable to generate ftable due to missing targetinfo data!')
        exit(1)

    f = open(ti, 'r')
    target = None

    for line in f:
        if is_match('^Target: ', line):
            target = Target(line[8:-1])
            target.parse(f)
        elif is_match('^Target-Profile:', line):
            profile = Profile(line[16:-1], target)
            profile.parse(f)
            list_profiles.append(profile)

    f.close()

class Feature:
    def __init__(self, name):
        self.name = empty_none_value(name)
        self.title = ''
        self.maintainer = ''
        self.packages = []
        self.desc = ''
        self.detail = ''

    def parse(self, file):
        help_parse = False
        detail_parse = False

        while True:
            position = file.tell()
            l = file.readline()

            if not l:
                break

            if is_match('^config ', l):
                file.seek(position)
                break
            elif 'bool ' in l or 'tristate ' in l:
                help_parse = False
                detail_parse = False
                tmp = l.split(' ', 1)

                if len(tmp) >= 2:
                    self.title = tmp[1][:-1].strip('"')
            elif 'select ' in l or 'imply ' in l:
                help_parse = False
                detail_parse = False
                info = l.split(' ', 2)

                if 'PACKAGE_' in info[1]:
                    self.packages.append(info[1][8:].strip())
                else:
                    self.packages.append(info[1][:-1])
            elif 'default ' in l:
                help_parse = False
                detail_parse = False
            elif 'maintainer ' in l:
                help_parse = False
                detail_parse = False
                self.maintainer = l[13:-2]
            elif 'detail' in l:
                help_parse = False
                detail_parse = True
                self.detail = l[7:]
            elif 'help' in l:
                help_parse = True
                detail_parse = False
                self.desc = l[5:]
            else:
                if help_parse is True:
                    self.desc = self.desc + l
                elif detail_parse is True:
                    self.detail = self.detail + l

        self.desc = ''.join(self.desc.split('\n\t', 1))
        self.desc = ''.join(self.desc.split('\t')).strip()

        self.detail = ''.join(self.detail.split('\n\t', 1))
        self.detail = ''.join(self.detail.split('\t')).strip()


def initialize_features():
    confs = [
        'config/Config-tlt.in',
        'target/Config.in'
    ]

    for c in confs:
        f = open(c)

        while True:
            line = f.readline()

            if not line:
                break

            if is_match('^config ', line):
                feature = Feature(line[7:-1])
                feature.parse(f)
                list_features.append(feature)

        f.close()

def dump_features(data):
    features = []

    for f in data:
        info = {
            'name': f.name,
            'title': f.title,
            'maintainer': f.maintainer,
            'description': f.desc,
            'detail': f.detail,
            'packages': []
        }

        for i in f.packages:
            pkg = find_pkg_by_name(i)

            if pkg is None:
                continue

            info2 = {
                'name': pkg.name,
                'title': pkg.title,
                'maintainer': pkg.maint,
                'description': pkg.desc,
                'detail': pkg.detail,
                'depends': pkg.deps, #self.find_package_deps(pkg.deps),
            }
            info['packages'].append(info2)

        features.append(info)

    return features

class Package:
    def __init__(self, name):
        self.name = name
        self.title = ''
        self.desc = ''
        self.detail = ''
        self.maint = ''
        self.deps = []

    def parse_dep_info(self, line):
        deps = line.split()
        parsed = []

        for d in deps:
            name = d
            is_not = False

            if '!' in d[1]:
                name = d[1:]
                is_not = True
            
            if '+' in name[0]:
                name = name[1:]

            if ':' in name:
                name = name.split(':')[1]

            parsed.append(name)

        return parsed

    def parse(self, file):
        detail_parse = False
        help_parse = False

        for l in file:
            if is_match('^@@$', l):
                break
            elif is_match('^Title: ', l):
                self.title = l[7:-1]
            elif is_match('^Maintainer: ', l):
                self.maint = l[12:-1]
            elif is_match('^Detail-Description: ', l):
                self.detail = l[20:-1].strip()
                detail_parse = True
            elif is_match('^Description: ', l):
                self.desc = l[13:-1].strip()
                help_parse = True
                detail_parse = False
            elif is_match('^Depends: ', l):
                self.deps = self.parse_dep_info(l[9:-1])
            else:
                if help_parse is True:
                    self.desc = self.desc + l
                elif detail_parse is True:
                    self.detail = self.detail + l

def initialize_packageinfo():
    for file in os.listdir('tmp/info'):
        if not '.packageinfo-' in file:
            continue

        f = open(f'tmp/info/{file}')

        for line in f:
            if is_match('^Package: ', line):
                pkg = Package(line[9:-1])
                pkg.parse(f)
                list_packages.append(pkg)

        f.close()


if len(sys.argv) >= 1 and 'dump' in sys.argv:
    dump_list = True

if not dump_list:
    initialize_targets()

initialize_features()
initialize_packageinfo()

if dump_list:
    print(json.dumps(dump_features(list_features), indent=2))
else:
    for i in list_profiles:
        print(i.json() + ',')
