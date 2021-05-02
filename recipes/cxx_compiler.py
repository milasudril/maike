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

rev_constants = {'c++98': 199711, 'c++11': 201103, 'c++14': 201402, 'c++17': 201703, 'c++17': 201702, 'c++20': 202002}

def configure(cfg):
	if 'std_revision' in cfg:
		rev = cfg['std_revision']
		if rev['min'] == rev['max']:
			rev['selected'] = rev['min']
			print(json.dumps(cfg))
		return 0
	else:
		return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))

	if sys.argv[1] == 'configure':
		exit(configure(json.loads(sys.argv[2])))
