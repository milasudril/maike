#@	{
#@	"targets":
#@		[
#@			 {"name":"maikeinfo.cpp","dependencies":[]}
#@			,{
#@			 "name":"maikeinfo.o","type":"object","source_name":"maikeinfo.cpp"
#@			,"source_generated":1
#@			,"dependencies":[{"ref":"maikeinfo.hpp","rel":"internal"}]
#@			}
#@		]
#@	}

import sys
import subprocess
import time
import string
import json

def write_error(*args, **kwargs):
	print(*args,file=sys.stderr,**kwargs)

def gitVersionGet():
	with subprocess.Popen(('git', 'describe','--tags','--dirty','--always')\
		,stdout=subprocess.PIPE) as git:
		result=git.stdout.read().decode().strip()
		git.wait()
		status=git.returncode

	if status:
		with open('versioninfo.txt') as versionfile:
			result=versionfile.read().strip()
	else:
		with open('versioninfo.txt','w') as versionfile:
			versionfile.write(result)

	return result

template=string.Template('''// This file has been generated by $scriptname

#include "../maikeinfo.hpp"

using namespace Maike;
using namespace Info;

const char* Info::VERSION="$version";
const char* Info::TIMESTAMP="$now";
''')

try:
#	sysvars=json.load(sys.stdin) Cannot be used, since we must be able to compile
#	Maike through a simple shell script :-(
	filename_out=sys.argv[1]+'/maikeinfo.cpp'
	keys=dict()
	keys['scriptname']=sys.argv[0]
	keys['version']=gitVersionGet()
	keys['now']=time.strftime('%Y-%m-%d %H:%M %Z')

	with open(filename_out,mode='w') as filehandle:
		filehandle.write(template.substitute(keys))
	sys.exit(0)

except Exception:
	write_error('%s: error: %s\n'%(sys.argv[0],sys.exc_info()[1]))
	sys.exit(2)

