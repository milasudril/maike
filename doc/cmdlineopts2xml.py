#!/usr/bin/env python3

#@	{
#@	 "target": {"name":"cmdlineopts.xml"}
#@	,"dependencies":
#@		[
#@		{"ref":"./cmdlineopts.md"}
#@		]
#@	}

import sys
import json
import os
import subprocess

def compile(args):
	target_dir = args['build_info']['target_dir']
	src_dir = args['build_info']['source_dir']
	targets = args['targets']
	src_file = target_dir + '/' + os.path.relpath(os.path.dirname(args['source_file']), src_dir) + '/cmdlineopts.md'
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-t', 'html', '--section-divs', src_file], text=True, stdout=subprocess.PIPE)

	doc = '''<content>
<appendix>
''' + pandoc_res.stdout + '''</appendix>
</content>
'''
	print(doc)

#	with open(targets[0], 'w') as f:
#		f.write('''<content>
#<appendix>
#''')
#		f.write(pandoc_res.stdout)
#		f.write()

	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))