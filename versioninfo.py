#@  {
#@  "targets":
#@      [{
#@		 "name":"versioninfo.txt"
#@		,"status_check":"dynamic"
#@		}]
#@  }

import sys
import subprocess
import shutil
import os

def git_changes():
	with subprocess.Popen(('git', 'status','--porcelain'),stdout=subprocess.PIPE) \
		as git:
		result=[];
		for k in filter(None,git.stdout.read().decode().split('\n')):
			result.append( k[3:].split(' ')[0] )
	return result

def get_revision():
	if shutil.which('git')==None:
		with open('versioninfo.txt') as versionfile:
			return versionfile.read().strip()

	else:
		with subprocess.Popen(('git', 'describe','--tags','--dirty','--always') \
			,stdout=subprocess.PIPE) as git:
			result=git.stdout.read().decode().strip()
			git.wait()
			status=git.returncode

		if status:
			with open('versioninfo-in.txt') as versionfile:
				return versionfile.read().strip()

		else:
			return result

def write_error(*args, **kwargs):
    print(*args,file=sys.stderr,**kwargs)

def get_rev_old():
	with os.fdopen(os.open(target_dir + '/versioninfo.txt',os.O_RDONLY|os.O_CREAT),'r+') \
		as verfile:
		return verfile.read().strip()

try:
	target_dir=sys.argv[1]
	in_dir=sys.argv[2]
	revision=get_revision()
	rev_old=get_rev_old()

	if rev_old!=revision:
		with open(target_dir + '/versioninfo.txt','w') as verfile:
			verfile.write(revision)

	with open('versioninfo-in.txt','w') as verfile:
		verfile.write(revision)

	sys.exit(0)
except Exception:
	write_error('%s:%d: error: %s\n'%(sys.argv[0],sys.exc_info()[2].tb_lineno,sys.exc_info()[1]))
	sys.exit(-1)
