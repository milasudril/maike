#!/usr/bin/env python3

#@	{
#@	 "target": {"name":"configdefault.xml"}
#@	,"dependencies":
#@		[
#@		 {"ref":"./configdefault.md"}
#@		,{"ref":"recipes/md2htmlpart.py", "origin":"project"}
#@		]
#@	}

import sys
import json
import os

def compile(args):
	src_dir = args['build_info']['source_dir']
	sys.path.insert(0, src_dir + '/recipes')
	import md2htmlpart
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	src_file = target_dir + '/' + os.path.relpath(os.path.dirname(args['source_file']), src_dir) + '/configdefault.md'
	return md2htmlpart.convert(src_file, targets[0])

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))