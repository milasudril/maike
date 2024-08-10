#!/usr/bin/env python3

#@	{
#@	 "target":
#@			{"name":"maike2.1.man"}
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
	src_dir = args['build_info']['source_dir']
	targets = args['targets']
	projinfo = load_projinfo(src_dir)
	content = dict()
	content['description'] = '\n\n'.join(projinfo['description'])
	content['author'] = ', '.join(get_authors(projinfo['copyright']))
	content['copyright'] = '\n\n'.join(get_copyright(projinfo['copyright']))
	content['license_short'] = projinfo['legal_info']['license_short']

	with open(target_dir + '/' + os.path.relpath(os.path.dirname(args['source_file']), src_dir) +'/cmdlineopts.md') as f:
		content['opts'] = f.read()

	timestamp = args['build_info']['start_time']
	content['date'] = datetime.datetime.fromtimestamp(timestamp, datetime.timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')
	md_output = document.substitute(content)
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-s', '-t', 'man', '-o', targets[0]], input=md_output, text=True)
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))