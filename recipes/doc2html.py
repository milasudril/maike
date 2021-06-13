#!/usr/bin/env python3
#@ {"dependencies":[{"ref":"./doc2html.xsl","origin":"project"}]}

import sys
import subprocess
import json
import xml.etree.ElementTree as ET
import os
from pathlib import Path

def compile(args):
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	source_file = args['source_file']
	current_dir =  target_dir + '/' + os.path.relpath(os.path.dirname(source_file), args['build_info']['source_dir'])
	with open(os.path.dirname(__file__) + '/doc2html.xsl') as f:
		cmd = ['xsltproc', '-o', targets[0], '--nonet', '--stringparam', 'current_dir', current_dir, '-', source_file]
		proc = subprocess.Popen(cmd, stdin=f, stderr=subprocess.PIPE)
		outs, errs = proc.communicate()
		if errs != b'':
			print(errs, file=sys.stderr)
			return 1
		return proc.returncode
	return 1

def get_tags(args):
	if args['source_file'] == __file__:
		return 0
	root = ET.parse(args['source_file'])
	deps = []
	for item in root.iter():
		if 'src' in item.attrib:
			dep = dict()
			dep['ref'] = item.attrib['src']
			dep['origin'] = 'generated'
			dep['rel'] = 'resource'
			deps.append(dep)

		if 'href' in item.attrib:
			dep = dict()
			dep['ref'] = item.attrib['href']
			dep['origin'] = 'generated'
			dep['rel'] = 'resource'
			deps.append(dep)

	target = dict()
	p = Path(os.path.basename(args['source_file']))
	extensions = ''.join(p.suffixes)
	target['name'] = str(p).replace(extensions, '.html')
	tags = dict()
	tags['dependencies'] = deps
	tags['target'] = target
	sys.stdout.write(json.dumps(tags))
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'get_tags':
		exit(get_tags(json.loads(sys.argv[2])))
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))