#!/usr/bin/env python3

#@	{
#@	 "targets":[{"name":"maike2.1.md"}, {"name":"maike2.1.man"}]
#@	,"dependencies":[{"ref":"src/bin/maike2"}, {"ref":"pandoc","origin":"system"}]
#@	}

import sys
import json
import subprocess
from string import Template
import datetime

document = Template('''---
title: MAIKE
section: 1
header: General Commands Manual
footer: maike
date: $date
---

# NAME
maike2

# DESCRIPTION
todo

# SYNOPSIS
**maike2** [*options*]

# OPTIONS

$opts
''')

def compile(args):
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	content = dict()
	maike_output = subprocess.run([target_dir + '/src/bin/maike2', '--help'], stdout=subprocess.PIPE, text=True)
	if maike_output.returncode != 0:
		return maike_output.returncode

	content['opts'] = maike_output.stdout.replace('--','\-\-')
	timestamp = args['build_info']['start_time']
	content['date'] = datetime.datetime.utcfromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S UTC')
	md_output = document.substitute(content)
	with open(targets[0], 'w') as f:
		f.write(md_output)
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-s', '-t', 'man', '-o', targets[1]], input=md_output, text=True)
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))