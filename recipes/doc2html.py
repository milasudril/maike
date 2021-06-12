#!/usr/bin/env python3

import sys
import subprocess
import json
import xml.etree.ElementTree as ET

def compile(args):
	target_dir = args['build_info']['target_dir']
	targets = args['targets']
	source_file = args['source_file']
	print(source_file)
	return 0

def get_tags(args):
	root = ET.parse(args['source_file'])
	deps = []
	for item in root.iter():
		if 'src' in item.attrib:
			dep = dict()
			dep['ref'] = item.attrib['src']
			dep['origin'] = item.attrib['maike-origin'] if 'maike-origin' in item.attrib else 'project'
			deps.append(dep)

		if 'href' in item.attrib:
			dep = dict()
			dep['ref'] = item.attrib['href']
			dep['origin'] = item.attrib['maike-origin'] if 'maike-origin' in item.attrib else 'project'

	tags = dict()
	tags['dependencies'] = deps
	sys.stdout.write(json.dumps(tags))
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'get_tags':
		exit(get_tags(json.loads(sys.argv[2])))
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))