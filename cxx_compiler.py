#!/usr/bin/env python3

import sys
import json
import os
import subprocess

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def format_iquote(list):
	ret = []
	for item in list:
		ret.append('-iquote%s'%(item))
	return ret

no_op = {'.hpp', '.h', '.hxx'}

def compile(dict):
	ext = os.path.splitext(dict['source_file'].lower())
	if ext[1] in no_op:
		return 0
	args = []
	args.append('g++')
	args.extend(dict['compiler_cfg']['cflags'])
	args.append('-std=c++17')
	args.extend(format_iquote(dict['compiler_cfg']['iquote']))
	args.append('-c')
	args.append(dict['source_file'])
	args.append('-o')
	args.append(dict['targets'][0])
	result = subprocess.run(args)
	return result.returncode


if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))
