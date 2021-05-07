#!/usr/bin/env python3

import subprocess
import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def get_cflags(libname):
	result = subprocess.run(['pkg-config', '--cflags', libname], stdout=subprocess.PIPE, Text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	return result.stdout.strip().split(' ')

def get_libs(libname):
	result = subprocess.run(['pkg-config', '--libs-only-l', libname], stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	ret = result.stdout.strip().split()
	ret.reverse()
	return ret

def get_libdirs(libname):
	result = subprocess.run(['pkg-config', '--libs-only-L', libname], stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	ret = result.stdout.strip().split()
	ret.reverse()
	return ret

if __name__ == '__main__':
	exit(0)