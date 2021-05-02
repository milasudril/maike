#!/usr/bin/env python3

#@	{"dependencies":[{"ref":"pkg_config.py","origin":"project"}]}

import sys
import json
import os
import subprocess
import shutil
import pkg_config

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def format_iquote(list):
	ret = []
	for item in list:
		ret.append('-iquote%s'%(item))
	return ret

def collect_cflags(compiler_flags, dependencies):
	ret = set()
	ret.update(compiler_flags['cflags'])
	ret.update(format_iquote(compiler_flags['iquote']))
	for item in dependencies:
		if item['origin'] == 'pkg-config':
			ret.update(pkg_config.get_cflags(item['ref']))
	ret.add('-std=c++17')
	return ret

no_op = {'.hpp', '.h', '.hxx'}

def compile(build_args):
	ext = os.path.splitext(build_args['source_file'].lower())
	if ext[1] in no_op:
		return 0
	args = []
	args.append('g++')
	args.extend(collect_cflags(build_args['compiler_cfg'], build_args['dependencies']))
	if 'std_revision' in build_args['compiler_cfg']:
		if 'selected' in build_args['compiler_cfg']['std_revision']:
			args.append('-std=%s'%build_args['compiler_cfg']['std_revision']['selected'])
	args.append('-fdiagnostics-color=%s'%('always' if build_args['log_format']=='ansi_term' else 'never'))
	args.append('-c')
	args.append(build_args['source_file'])
	args.append('-o')
	args.append(build_args['targets'][0])
	result = subprocess.run(args)

	for target in build_args['targets'][1:]:
		shutil.copyfile(build_args['targets'][0], target)

	return result.returncode

def get_numeric_rev(rev):
	rev_constants = {'c++98': 199711, 'c++11': 201103, 'c++14': 201402, 'c++17': 201703, 'c++17': 201702, 'c++20': 202002}
	return rev_constants[rev]

def get_literal_rev(rev):
	rev_constants = {199711:'c++98', 201103:'c++11', 201402:'c++14', 201703:'c++17', 202002:'c++20'}
	return rev_constants[rev]


def get_default_ref():
	args = ['g++', '-x', 'c++', '-E', '-dM', '/dev/null']
	result = subprocess.run(args, stdin = subprocess.DEVNULL, stdout=subprocess.PIPE, text=True)
	for line in result.stdout.split('\n'):
		fields=line.split()
		if len(fields) >= 3:
			if fields[1] == '__cplusplus':
				return int(fields[2][0:-1])


def select_cpp_rev(rev):
	if 'min' in rev:
		min_num = get_numeric_rev(rev['min'])
		if 'max' in rev:
			max_num = get_numeric_rev(rev['max'])
			if min_num > max_num:
				eprint('Impossible configuration (min %s is newer than max %s)'%(rev['min'], rev['max']))
				return 1
			if rev['min'] == rev['max']:
				rev['selected'] = rev['min']
				return 0
			else:
				default_rev = get_default_ref()
				if default_rev < min_num:
					rev['selected'] = rev['min']
					return 0
				if default_rev > max_num:
					rev['selected'] = rev['max']
					return 0
				return 0
		else:
			default_rev = get_default_ref()
			if default_rev < min_num:
				rev['selected'] = rev['min']
				return 0
	else:
		if 'max' in rev:
			default_rev = get_default_ref()
			if default_rev > max_num:
				rev['selected'] = rev['max']
				return 0

def configure(cfg):
	if 'std_revision' in cfg:
		ret = select_cpp_rev(cfg['std_revision'])
		print(json.dumps(cfg))
		return 0
	else:
		return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))

	if sys.argv[1] == 'configure':
		exit(configure(json.loads(sys.argv[2])))
