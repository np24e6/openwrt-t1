#!/usr/bin/env python3

import sys
import os
import subprocess
import json
import re
import argparse
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
        if ';' in value:
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

    def recursive_insert(self, dct, keys, value):
        kname = keys[0].replace('_', ' ')
        if len(keys) == 1:
            dct[kname] = value
        else:
            if kname not in dct:
                dct[kname] = {}
            self.recursive_insert(dct[kname], keys[1:], value)

    def convert_to_array_format(self, dct):
        result = []
        for key, value in dct.items():
            entry = {"title": key}
            if isinstance(value, dict):
                entry["detail"] = [{"key": subkey, "value": subvalue} for subkey, subvalue in value.items()]
            else:
                entry["detail"] = [value]
            result.append(entry)
        return result

    def parse(self, file):
        for l in file:
            if is_match('^@@$', l):
                break
            elif is_match('^Target-Profile-Name:', l):
                self.name = l[21:-1]
            elif is_match('^Target-Profile-Features: ', l):
                self.features = set(l[25:-1].split())
                self.features.update(self.target.features)
            elif 'Target-Profile-HARDWARE-' in l:
                if len(l.strip()[24:].split(': ')) != 2:
                    continue

                key, value = l.strip()[24:].split(': ')
                key_parts = key.split('-')
                self.recursive_insert(self.hw, key_parts, value)

        self.hw = self.convert_to_array_format(self.hw)

    def parse_expr(self, expr):
        pattern = re.compile(r'if\s+(.*)', re.DOTALL)
        match = pattern.match(expr)

        if not match:
            raise ValueError(f'Invalid detail line format: {expr}')

        condition_expr = match.group(1)

        if condition_expr:
            tokens = re.findall(r'\w+|&&|\|\||!|\(|\)', condition_expr)
        else:
            tokens = []

        return tokens

    def evaluate_expr(self, tokens):
        def eval_token(token):
            if token in self.conf:
                return self.conf[token] in {True, 'm'}
            return False

        def eval_not(token):
            return not eval_token(token)

        def eval_and(tokens):
            return all(self.evaluate_expr(t) for t in tokens)

        def eval_or(tokens):
            return any(self.evaluate_expr(t) for t in tokens)

        stack = []
        i = 0
        while i < len(tokens):
            token = tokens[i]
            if token == '&&':
                stack.append('&&')
            elif token == '||':
                stack.append('||')
            elif token == '!':
                stack.append(eval_not(tokens[i+1]))
                i += 1
            elif token == '(':
                sub_expr = []
                paren_count = 1
                i += 1
                while i < len(tokens):
                    if tokens[i] == '(':
                        paren_count += 1
                    elif tokens[i] == ')':
                        paren_count -= 1
                        if paren_count == 0:
                            break
                    sub_expr.append(tokens[i])
                    i += 1
                stack.append(self.evaluate_expr(sub_expr))
            else:
                stack.append(eval_token(token))
            i += 1

        result = stack[0]
        i = 1
        while i < len(stack):
            if stack[i] == '&&':
                result = result and stack[i+1]
            elif stack[i] == '||':
                result = result or stack[i+1]
            i += 2

        return result

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
                if line.endswith('is not set'):
                    self.conf[line.split(' ')[1].split('CONFIG_')[1]] = False
                elif '=' in line:
                    option, value = line.split('=', 1)
                    self.conf[option.split('CONFIG_')[1]] = True if value == 'y' else value

    def fetch_features(self):
        self.features = []

        for f in list_features:
            if not f.name in self.conf:
                continue

            new_detail = []

            for d in f.detail:
                new_item = {'key': d['key'], 'value': d['value']}

                if not d['expr']:
                    new_detail.append(new_item)
                    continue

                expr = self.parse_expr(d['expr'])
                if self.evaluate_expr(expr):
                    new_detail.append(new_item)

            f.detail = new_detail
            f.external = (self.conf[f.name] == 'm')

            self.features.append(f)

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

    def json(self, descriptions_only=False):
        data = {
            "hardware": self.hw,
        }
        if not descriptions_only:
            data = {
                "name": self.name,
                "target": {"name": self.target.target(), "subtarget": self.target.subtarget(), "profile": self.profile},
                "hardware": self.hw,
                "features": [],
            }

        self.prepare_dotconfig()
        self.read_config()
        self.fetch_features()

        data["features"] = dump_features(self.features, descriptions_only)

        return json.dumps(data, indent=2, ensure_ascii=False)

def initialize_targets(targetDevice=None, targetFamily=None):
    ti = 'tmp/.targetinfo'


    print('Initializing targets...')

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
            if targetDevice is not None:
                if targetDevice in line[33:-1]:
                    profile = Profile(line[16:-1], target)
                    profile.parse(f)
                    list_profiles.append(profile)
                    break
            elif targetFamily is not None:
                if targetFamily in line[33:-1]:
                    profile = Profile(line[16:-1], target)
                    profile.parse(f)
                    list_profiles.append(profile)
            else:
                profile = Profile(line[16:-1], target)
                profile.parse(f)
                list_profiles.append(profile)

    f.close()

class Feature:
    def __init__(self, name):
        self.name = empty_none_value(name)
        self.title = ''
        self.maintainer = ''
        self.external = False
        self.packages = []
        self.desc = ''
        self.detail = []

    def parse_detail(self, line, expr):
        l = line.strip()

        if not l:
            return

        key, value = '', ''

        if ':' in l:
            key, value = l.split(':', 1)
            key = key.strip()
            value = value.strip()
        else:
            value = l.strip()

            # update previous value
            if len(self.detail) > 0:
                previous = self.detail[-1]
                previous['value'] += ' ' + value
                return

        self.detail.append({'key': key, 'value': value, 'expr': expr})

    def parse(self, file):
        help_parse = False
        detail_parse = False
        detail_expr_parse = False
        detail_expr_str = ''

        while True:
            position = file.tell()
            l = file.readline()

            if not l:
                break

            if is_match('^config ', l):
                file.seek(position)
                break
            elif is_match('^\tbool ', l) or is_match('^\ttristate ', l):
                help_parse = False
                detail_parse = False
                detail_expr_parse = False
                tmp = l.split(' ', 1)

                if len(tmp) >= 2:
                    self.title = tmp[1][:-1].strip('"')
            elif is_match('^\tselect ', l) or is_match('^\timply ', l):
                help_parse = False
                detail_parse = False
                detail_expr_parse = False
                info = l.split(' ', 2)

                if 'PACKAGE_' in info[1]:
                    self.packages.append(info[1][8:].strip())
                else:
                    self.packages.append(info[1][:-1])
            elif is_match('^\tdefault ', l):
                help_parse = False
                detail_parse = False
                detail_expr_parse = False
            elif is_match('^\tmaintainer ', l):
                help_parse = False
                detail_parse = False
                self.maintainer = l[13:-2]
            elif is_match('^\tdepends on ', l):
                help_parse = False
                detail_parse = False
                detail_expr_parse = False
            elif is_match('^\tdetail', l):
                help_parse = False
                detail_parse = True
                if 'detail if ' in l:
                    detail_expr_str = l[7:].strip()
                    if l[:-1].strip().endswith('\\'):
                        detail_expr_parse = True
                        detail_expr_str = detail_expr_str[:-2]
                else:
                    detail_expr_str = ''
            elif is_match('^\thelp', l):
                help_parse = True
                detail_parse = False
                detail_expr_parse = False
                self.desc = l[5:]
            elif l.lstrip().startswith('#'):
                continue
            else:
                if help_parse is True:
                    self.desc = self.desc + l
                elif detail_parse is True:
                    l = l.strip()

                    if not l:
                        continue

                    if detail_expr_parse:
                        detail_expr_str += ' ' + l.strip()
                        if not l[:-1].strip().endswith('\\'):
                            detail_expr_parse = False
                        continue

                    self.parse_detail(l, detail_expr_str)

        self.desc = ''.join(self.desc.split('\n\t', 1))
        self.desc = ''.join(self.desc.split('\t')).strip()

def initialize_features():
    confs = [
        'config/Config-tlt.in',
        'target/Config.in'
    ]

    print('Initializing features...')

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

def dump_features(data, descriptions_only=False):
    features = []

    for f in data:
        info = {
            "title": f.title,
            "description": f.desc,
            "detail": [{'key': item['key'], 'value': item['value']} for item in f.detail],
        }

        if descriptions_only:
            if f.title:
                features.append(info)
            continue

        info["name"] = f.name
        info["maintainer"] = f.maintainer
        info["external"] = f.external
        info["packages"] = list()

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
            info["packages"].append(info2)

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

            if '!' in d[1]:
                name = d[1:]

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
                tmp = l[20:-1].strip()
                if tmp == self.title:
                    continue
                detail_parse = True
                self.detail = tmp
            elif is_match('^Description: ', l):
                tmp = l[13:-1].strip()
                if tmp == self.title:
                    continue
                self.desc = tmp
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
    print('Initializing packageinfo...')

    for file in os.listdir('tmp/info'):
        if ".packageinfo-" not in file:
            continue

        f = open(f'tmp/info/{file}')

        for line in f:
            if is_match('^Package: ', line):
                pkg = Package(line[9:-1])
                pkg.parse(f)
                list_packages.append(pkg)

        f.close()


def generate_device_features(dump=False, descriptions_only=False, target=None, family=None):
    default_dir='out'
    default_name='features.json'
    dump_list = False

    print('Generating device features...')

    if dump:
        print('Dumping all possible device features.')
        dump_list = True
    if target:
        print(f'Generating features for target device: {target}')
        default_name = f'{target}.json'
    if family:
        print(f'Generating features for device family: {family}')
        default_name = f'{family}.json'

    if not dump_list:
        initialize_targets(targetDevice=target, targetFamily=family)

    initialize_features()
    initialize_packageinfo()

    if not os.path.exists(default_dir):
        os.makedirs(default_dir)

    if dump_list:
        json_data = json.dumps(dump_features(list_features, descriptions_only), indent=2, ensure_ascii=False)
    else:
        data = []
        for p in list_profiles:
            print(f'Generating {p.name}...')
            data.append(p.json(descriptions_only))

        json_data = ',\n'.join(data)

    if len(json_data) == 0:
        print('Unable to generate json data!')
        print('Target, family is invalid or project files are corrupted!')
        return

    default_path = os.path.join(default_dir, default_name)
    with open(default_path, 'w') as file:
        file.write(json_data)

    print(f'Data written to {default_path} path.')

def main():
    parser = argparse.ArgumentParser(description='Generate device features.')
    parser.add_argument("--descriptions-only", action="store_true", help="Generate only descriptions of features")

    group = parser.add_mutually_exclusive_group()
    group.add_argument('--dump', action='store_true', help='Generate all possible device features')
    group.add_argument('--target', type=str, help='Generate device features for a specific target device')
    group.add_argument('--family', type=str, help='Generate device features for a specific device family')

    args = parser.parse_args()

    if not any([args.dump, args.target, args.family]):
        parser.print_help()
        sys.exit(1)

    generate_device_features(args.dump, args.descriptions_only, args.target, args.family)


if __name__ == '__main__':
    main()
