#!/usr/bin/env python3

#@	{
#@	 "target":{"name":"about_impl.hpp"}
#FIXME: should be sufficient to use `maike.vcslog` to get path relative to projroot
#@	,"dependencies":[{"ref":"../../maike.vcslog","origin":"generated"}]
#@	,"rebuild_policy":"always"
#@	}

import sys
import json
from string import Template
import datetime

buildinfo = Template('''char const* Maike::Self::BuildId = "$build_id";

char const* Maike::Self::BuildStartTime = "$start_time";
''')

vcsinfo = Template('''char const* Maike::Self::VcsRevisionId = "$commit";

char const* Maike::Self::VcsTag = "$tag";
''')

file_content = Template('''#ifndef MAIKE_BIN_ABOUTIMPL_HPP
#define MAIKE_BIN_ABOUTIMPL_HPP

$build_info
$vcs_info

char const* Maike::Self::Copyright = "Torbjörn Rathsman © 2016-2021";

char const* Maike::Self::DescriptionShort = "A build system based on scanning of source files";

char const* Maike::Self::LegalBrief = R"STR(Maike comes with ABSOLUTELY NO WARRANTY; for details see section 15 and 16 in the GNU General Public License version 3. Maike is free software, and you are welcome to redistribute it under the terms of the GNU General Public License, either version 3, or at your option, any later version.)STR";

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