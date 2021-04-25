#!/usr/bin/env python3

import sys
import json
import os

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def mkdir(path):
	try:
		if path.endswith('/.'):
			os.mkdir(path[0: -2])
		else:
			os.mkdir(path)
	except FileExistsError:
		pass

def compile(dict):
	for target in dict["targets"]:
		mkdir(target)

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		compile(json.loads(sys.argv[2]))
