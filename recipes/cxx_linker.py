#!/usr/bin/env python3

#@	{"dependencies":[{"ref":"pkg_config.py","origin":"project"}]}

import sys
import json
import os
import subprocess
import pkg_config

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def collect_deps(deps):
	ret = []
	for item in deps:
		if (item['origin'] == 'generated' or item['origin'] == 'project') and 'rel' in item:
			if item['rel'] == 'implementation':
				ret.append(item['ref'])
		elif item['origin'] == 'pkg-config':
			ret.extend(pkg_config.get_libs(item['ref']))
			ret.extend(pkg_config.get_libdirs(item['ref']))
		elif item['origin'] == 'system' and 'rel' in item:
			if item['rel'] == 'external':
				ret.append('-l%s'%item['ref'])
	return ret

def filter_deps(dep_items):
	bookkeeping = set()
	ret = []
	for item in dep_items:
		if not item in bookkeeping:
			ret.append(item)
			bookkeeping.add(item)
	ret.reverse()
	return ret


def compile(build_args):
#	TODO: different rule for dll?
#	ext = os.path.splitext(dict['source_file'].lower())
#
	args = []
	args.append('g++')
	args.extend(build_args['compiler_cfg']['cflags'])
	args.append('-fdiagnostics-color=%s'%('always' if build_args['log_format']=='ansi_term' else 'never'))
	args.extend(filter_deps(collect_deps(build_args['dependencies'])))
	args.append('-o')
	args.append(build_args['targets'][0])
	result = subprocess.run(args)
	return result.returncode

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))
