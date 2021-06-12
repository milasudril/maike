#!/usr/bin/env python3

#@	{
#@	 "target": {"name":"cmdlineopts.xml"}
#@	,"dependencies":
#@		[
#@		{"ref":"./cmdlineopts.md"}
#@		]
#@	}

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

def compile(args):
	target_dir = args['build_info']['target_dir']
	src_dir = args['build_info']['source_dir']
	targets = args['targets']
	src_file = target_dir + '/' + os.path.relpath(os.path.dirname(args['source_file']), src_dir) + '/cmdlineopts.md'
	pandoc_res = subprocess.run(['pandoc', '-f', 'markdown', '-t', 'html', '--section-divs', src_file], text=True, stdout=subprocess.PIPE)

	doc = '''<content>
<appendix>
''' + pandoc_res.stdout + '''</appendix>
</content>
'''

	root = ET.fromstring(doc)
	for section in find_elements(root, 'section', []):
		for elem in section.findall('./*'):
			if elem.tag.startswith('h'):
				section.attrib['title'] = elem.text
				section.attrib.pop('class', None)
				section.remove(elem)
				break

	ET.ElementTree(root).write(targets[0])
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))