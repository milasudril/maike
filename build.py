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
		args = ['sh', '-x', '__buildcache/bootstrap.sh']
		result = subprocess.run(args)
		return result.returncode

def rebuild():
	args = ['__targets_bootstrap/core/bin/maike2', '--target-dir=__targets']
	result = subprocess.run(args)
	return result.returncode

if __name__ == '__main__':
	if len(sys.argv) >= 2:
		if sys.argv[1] == 'get_tags' or sys.argv[1] == 'configure' or sys.argv[1] == 'compile':
			exit(0)

	result = do_initial_build()
	if result != 0:
		exit(result)

	result = rebuild()
	if result != 0:
		exit(result)