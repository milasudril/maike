#!/usr/bin/env python3

#@	{
#@	 "target":{"name":"about_impl.hpp"}
#FIXME: should be sufficient to use `maike.vcslog` to get path relative to projroot
#@	,"dependencies":[{"ref":"../../maike.vcslog","origin":"generated"}]
#@	}

import sys
import json
from string import Template
import datetime

buildinfo = Template('''char const* BuildId = "$build_id";

char const* BuildStartTime = "$start_time";
''')

vcsinfo = Template('''char const* VcsRevisionId = "$commit";

char const* VcsTag = "$tag";
''')

file_content = Template('''#ifndef MAIKE_BIN_ABOUTIMPL_HPP
#define MAIKE_BIN_ABOUTIMPL_HPP

$build_info
$vcs_info
#endif
''')

def get_vcsinfo(target_dir):
	with open(target_dir + '/maike.vcslog') as f:
		return json.load(f)

def compile(args):
	output = dict()
	timestamp = args['build_info']['start_time']
	args['build_info']['start_time'] = datetime.datetime.utcfromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S UTC')
	output['build_info'] = buildinfo.substitute(args['build_info'])
	output['vcs_info'] = vcsinfo.substitute(get_vcsinfo(args['build_info']['target_dir']))
	output_str = file_content.substitute(output)
	with open(args['targets'][0], 'w') as f:
		f.write(output_str)
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))