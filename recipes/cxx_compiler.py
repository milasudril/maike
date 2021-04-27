#!/usr/bin/env python3

import sys
import json
import os
import subprocess
import shutil

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def format_iquote(list):
	ret = []
	for item in list:
		ret.append('-iquote%s'%(item))
	return ret

def get_pkg_config(libname):
	result = subprocess.run(['pkg-config', '--cflags', libname], stdout=subprocess.PIPE, Text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	return result.stdout.strip().split(' ')

def collect_cflags(compiler_flags, dependencies):
	ret = set()
	ret.update(compiler_flags['cflags'])
	eprint(compiler_flags['cflags'])
	ret.update(format_iquote(compiler_flags['iquote']))
	for item in dependencies:
		if item['origin'] == 'pkg-config':
			ret.update(get_pkg_config(item['ref']))
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
	args.append('-std=c++17')
	args.append('-c')
	args.append(build_args['source_file'])
	args.append('-o')
	args.append(build_args['targets'][0])
	result = subprocess.run(args)

	for target in build_args['targets'][1:]:
		shutil.copyfile(build_args['targets'][0], target)

	return result.returncode


if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))
