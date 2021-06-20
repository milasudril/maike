#!/usr/bin/env python3

#@	{"dependencies": [{"ref":"pandoc", "origin":"system"}]}

import sys
import json
import os
import subprocess
import xml.etree.ElementTree as ET

def find_elements(node, element, result):
	for item in node.iter():
		if item.tag == element:
			result.append(item)
	return result

def convert(src_file, target):
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-t', 'html', '--section-divs', src_file], text=True, stdout=subprocess.PIPE)

	doc = '<content>' + pandoc_res.stdout + '</content>'

	root = ET.fromstring(doc)
	for section in find_elements(root, 'section', []):
		for elem in section.findall('./*'):
			if elem.tag.startswith('h'):
				section.attrib['title'] = elem.text
				section.attrib.pop('class', None)
				section.remove(elem)
				break

	ET.ElementTree(root).write(target)
	return 0

def compile(args):
	targets = args['targets']
	src_file = args['source_file']
	return convert(src_file, targets[0])

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))