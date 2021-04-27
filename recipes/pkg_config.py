import subprocess

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def get_cflags(libname):
	result = subprocess.run(['pkg-config', '--cflags', libname], stdout=subprocess.PIPE, Text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	return result.stdout.strip().split(' ')

def get_libs(libname):
	result = subprocess.run(['pkg-config', '--libs', libname], stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('pkg-config failed: %d' % result.returncode)
	return result.stdout.strip().split(' ')

if __name__ == '__main__':
	eprint('Not intended to be run as a standalone script')
	exit(1)