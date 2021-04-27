#!/usr/bin/env python3

#@	{"dependencies":[{"ref":"pkg_config.py","origin":"project"}]}

import sys
import json
import os
import subprocess
import pkg_config

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def build_dep_array(list):
	ret = []
	for item in list:
		if (item['origin'] == 'generated' or item['origin'] == 'project') and 'rel' in item:
			if item['rel'] == 'implementation':
				ret.append(item['ref'])
		elif item['origin'] == 'pkg-config':
			ret.extend(pkg_config.get_libs(item['ref']))
		elif item['origin'] == 'system':
			ret.append('-l%s'%item['ref'])

	return ret

def compile(dict):
#	TODO: different rule for dll?
#	ext = os.path.splitext(dict['source_file'].lower())
#
	args = []
	args.append('g++')
	args.extend(dict['compiler_cfg']['cflags'])
	args.append('-std=c++17')
	args.extend(build_dep_array(dict['dependencies']))
	args.append('-o')
	args.append(dict['targets'][0])
	result = subprocess.run(args)
	return result.returncode

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))
