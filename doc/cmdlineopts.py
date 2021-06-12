#!/usr/bin/env python3

#@	{
#@	 "target": {"name":"cmdlineopts.md"}
#@	,"dependencies":
#@		[
#@		{"ref":"core/bin/maike2"}
#@		]
#@	}

import sys
import subprocess
import json

def compile(args):
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	content = dict()
	maike_output = subprocess.run([target_dir + '/core/bin/maike2', '--help'], stdout=subprocess.PIPE, text=True)
	if maike_output.returncode != 0:
		return maike_output.returncode

	with open(targets[0], 'w') as f:
		f.write(maike_output.stdout.replace('--','\-\-'))

	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))