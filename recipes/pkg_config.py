#!/usr/bin/env python3

import subprocess
import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def get_cflags(libname):
	result = subprocess.run(['pkg-config', '--cflags', libname], stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	ret_str = result.stdout.strip()
	if len(ret_str) == 0:
		return []
	return ret_str.split(' ')

def get_libs(libname):
	result = subprocess.run(['pkg-config', '--libs-only-l', libname], stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	ret_str = result.stdout.strip()
	if len(ret_str) == 0:
		return []
	ret = ret_str.split()
	ret.reverse()
	return ret

def get_libdirs(libname):
	result = subprocess.run(['pkg-config', '--libs-only-L', libname], stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	ret_str = result.stdout.strip()
	if len(ret_str) == 0:
		return []
	ret = ret_str.split()
	ret.reverse()
	return ret

if __name__ == '__main__':
	exit(0)