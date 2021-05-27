#!/usr/bin/env python3

import sys
import json
import os
import subprocess

compiler = os.environ['AR'] if 'AR' in os.environ else 'ar'

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def collect_deps(deps):
	ret = []
	for item in deps:
		if (item['origin'] == 'generated' or item['origin'] == 'project') and 'rel' in item:
			if item['rel'] == 'implementation':
				ret.append(item['ref'])
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
	args = [compiler, 'rcs']
	args.append(build_args['targets'][0])
	args.extend(filter_deps(collect_deps(build_args['dependencies'])))
	result = subprocess.run(args)
	return result.returncode

if __name__ == '__main__':
	try:
		if sys.argv[1] == 'compile':
			exit(compile(json.loads(sys.argv[2])))
	except Exception as e:
		print(e, file=sys.stderr)
		exit(-1)