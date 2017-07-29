#@	{
#@	"targets":
#@		[{
#@		"name":"maike.man1","dependencies":
#@			[
#@			 {"ref":"../projectinfo.json","rel":"misc"}
#@			,{"ref":"options.troff","rel":"misc"}
#@			]
#@		}]
#@	}

import sys
import string
import json
import os
import time

def write_error(*args, **kwargs):
    print(*args,file=sys.stderr,**kwargs)

doc=string.Template('''.TH $NAME 1 $now $source "General Commands"
.SH NAME
$name \- $description
.SH SYNOPSIS
.B $name_lowercase [options]
.SH DESCRIPTION
$description_long
.SH OPTIONS
$options
.SH ENVIRONMENT
.B $name
does not use any environment variables itself other that \\fBPATH\\fR. Notice however, that programs invoked by $name, may be affected by different environment variables.
.SH BUGS
.IP \(bu 4
Currently, $name only support single-threaded compilation.
.IP \(bu 4
Invoking $name from a script extension may result in a corrupted build, and an infinite process creation.
.SH SEE ALSO
.IP \(bu 4
\\fBbash\\fR(1), \\fBg++\\fR(1), \\fBoctave-cli\\fR(1), \\fBpython3\\fR(1)
.IP \(bu 4
Maike user's guide availible at https://milasudril.github.io/maike
''')


def load_json(filename):
	with open(filename,encoding='utf-8') as input:
		return json.load(input)

def options(src):
	with open(src) as file:
		return file.read()
try:
	target_dir=sys.argv[1]
	in_dir=sys.argv[2]

	subst=load_json('projectinfo.json')
	subst['source']=subst['name'].lower() + '/' + in_dir
	subst['name_lowercase']=subst['name'].lower()
	subst['NAME']=subst['name'].upper()
	subst['options']=options(target_dir+'/'+in_dir+'/options.troff')
	subst['now']=time.strftime('%Y-%m-%d')

	with open(target_dir + '/' + in_dir + '/maike.man1','wb') as output:
		output.write(doc.substitute(subst).encode('utf-8'))

	sys.exit(0)

except Exception:
	write_error('%s:%d: error: %s\n'%(sys.argv[0],sys.exc_info()[2].tb_lineno,sys.exc_info()[1]))
	sys.exit(-1)
