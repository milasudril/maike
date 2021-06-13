#!/usr/bin/env python3

import sys
import json
import shutil

def compile(args):
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	source_file = args['source_file']
	for target in targets:
		print('%s %s'%(source_file, target))
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))