#!/usr/bin/env python3

import sys
import json
import subprocess
from string import Template

def get_tags(args, scriptname):
	ret = dict()
	if args['source_file'] == scriptname:
		return 0

	ret['target'] = dict()
	compiler_cfg = args['compiler_cfg']
	ret['target']['name']=args['source_file'] + '.' +  compiler_cfg['target_filename_ext']

	command = compiler_cfg['compilation_cmd']
	ret['dependencies'] = [{'ref': command['executable'], 'origin': command['exec_origin']}]
	print(json.dumps(ret))
	return 0

def compile(args):
	print(args, file=sys.stderr)
	cmd_args = []
	compiler_cfg = args['compiler_cfg']
	command = compiler_cfg['compilation_cmd']
	cmd_args.append(command['executable'])
	subst = dict()
	subst['source_file'] = args['source_file']
	subst['target'] = args['targets'][0]
	for item in command['args']:
		arg_template = Template(item)
		cmd_args.append(arg_template.substitute(subst))

	result = subprocess.run(cmd_args)
	return result.returncode

if __name__ == '__main__':
	args = json.loads(sys.argv[2])
	if sys.argv[1] == 'get_tags':
		exit(get_tags(args, sys.argv[0]))
	if sys.argv[1] == 'configure':
		exit(0)
	if sys.argv[1] == 'compile':
		compile(args);