#@	{
#@	"targets":
#@		[
#@			 {"name":"maikeinfo.cpp","dependencies":[]}
#@			,{
#@			 "name":"maikeinfo.o","type":"object","source_name":"maikeinfo.cpp"
#@			,"source_generated":1
#@			,"dependencies":[{"ref":"./maikeinfo.hpp","rel":"internal"}]
#@			}
#@		]
#@	}

import sys
import subprocess
import time
import string

def write_error(*args, **kwargs):
	print(*args,file=sys.stderr,**kwargs)

def gitVersionGet():
	with subprocess.Popen(('git', 'describe','--tags','--dirty','--always')\
		,stdout=subprocess.PIPE) as git:
		return git.stdout.read().decode().strip()

template=string.Template('''// This file has been generated by $scriptname

#include "../maikeinfo.hpp"

using namespace Maike;
using namespace Info;

const char* Info::VERSION="$version";
const char* Info::TIMESTAMP="$now";
''')

try:
	filename_out=sys.argv[1]+'/maikeinfo.cpp'
	keys=dict()
	keys['scriptname']=sys.argv[0]
	keys['version']=gitVersionGet()
	keys['now']=time.strftime('%Y-%m-%d %H:%M')

	with open(filename_out,mode='w') as filehandle:
		filehandle.write(template.substitute(keys))
except:
	write_error('%s: %s\n'%(sys.argv[0],sys.exc_info()[1]))


#for arg in sys.argv:
#	print(arg)
#print('Hello, World')
