#!/usr/bin/env python3

import sys
import json
import sass

def compile(args):
	targets = args['targets']
	print(targets[0])
	source_file = args['source_file']
	with open(source_file) as f:
		result = sass.compile(string = f.read())

	for target in targets:
		with open(target, 'w') as f:
			f.write(result)
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))