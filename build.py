#!/usr/bin/python3

import subprocess
import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def do_initial_build():
	try:
		args = ['maike2', '--target-dir=__targets_bootstrap']
		result = subprocess.run(args)
		return result.returncode
	except:
		args = ['sh', '-x', '__targets/bootstrap.sh']
		result = subprocess.run(args)
		return result.returncode

def rebuild():
	args = ['__targets_bootstrap/src/bin/maike2', '--target-dir=__targets']
	result = subprocess.run(args)
	return result.returncode

if __name__ == '__main__':
	if len(sys.argv) >= 2 and sys.argv[1] == 'configure':
		eprint('This script should not be run from maike2')
		exit(1)

	result = do_initial_build()
	if result != 0:
		exit(result)

	result = rebuild()
	if result != 0:
		exit(result)