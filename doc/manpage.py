#!/usr/bin/env python3

#@	{
#@	 "targets":
#@		[
#@			{"name":"maike2.1.md"},
#@			{"name":"maike2.1.man"},
#@			{"name":"cmdline.xml"}
#@		]
#@	,"dependencies":
#@		[
#@		{"ref":"./cmdlineopts.md"},
#@		{"ref":"pandoc","origin":"system"},
#@		{"ref":"projinfo.json", "origin":"project"}
#@		]
#@	}

import sys
import json
import subprocess
from string import Template
import datetime
import os

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
$description

# SYNOPSIS
**maike2** [*options*]

# OPTIONS

$opts

# AUTHOR

$author

# COPYRIGHT

$copyright

$license_short
''')

def load_projinfo(src_dir):
	with open(src_dir + '/projinfo.json') as f:
		return json.load(f)

def get_authors(projinfo_copyright):
	ret = []
	for item in projinfo_copyright:
		ret.append(item['author'])
	ret.sort()
	return ret

def get_copyright(projinfo_copyright):
	ret = []
	for item in projinfo_copyright:
		years_str = []
		for year in sorted(item['years']):
			years_str.append(str(year))
		ret.append('© %s : %s'%(', '.join(years_str), item['author']))
	return ret

def compile(args):
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	projinfo = load_projinfo(args['build_info']['source_dir'])
	content = dict()
	content['description'] = '\n\n'.join(projinfo['description'])
	content['author'] = ', '.join(get_authors(projinfo['copyright']))
	content['copyright'] = '\n\n'.join(get_copyright(projinfo['copyright']))
	content['license_short'] = projinfo['legal_info']['license_short']

	with open(target_dir + '/' + os.path.dirname(args['source_file']) +'/cmdlineopts.md') as f:
		content['opts'] = f.read()

	timestamp = args['build_info']['start_time']
	content['date'] = datetime.datetime.utcfromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S UTC')
	md_output = document.substitute(content)
	with open(targets[0], 'w') as f:
		f.write(md_output)
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-s', '-t', 'man', '-o', targets[1]], input=md_output, text=True)
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-t', 'html', '--section-divs', '-o', targets[2]], input=content['opts'], text=True)
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))