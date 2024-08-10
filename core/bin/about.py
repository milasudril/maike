#!/usr/bin/env python3

#@	{
#@	 "target":{"name":"about_impl.hpp"}
#@	,"dependencies":[{"ref":"maike.vcslog","origin":"generated"}, {"ref":"projinfo.json", "origin":"project"}]
#@	,"rebuild_policy":"always"
#@	}

import sys
import json
from string import Template
import datetime
import socket

buildinfo = Template('''char const* Maike::Self::BuildId = "$build_id";

char const* Maike::Self::BuildStartTime = "$start_time";

char const* Maike::Self::BuildHost = "$build_host";
''')

vcsinfo = Template('''char const* Maike::Self::VcsRevisionId = "$commit";

char const* Maike::Self::VcsTag = "$tag";
''')

file_content = Template('''#ifndef MAIKE_BIN_ABOUTIMPL_HPP
#define MAIKE_BIN_ABOUTIMPL_HPP

$build_info

$vcs_info

char const* Maike::Self::Copyright = "$copyright";

char const* Maike::Self::DescriptionShort = "$description_short";

char const* Maike::Self::LegalBrief = "$legal_brief";

#endif
''')

def get_vcsinfo(target_dir):
	with open(target_dir + '/maike.vcslog') as f:
		return json.load(f)

def get_projinfo(source_dir):
	with open(source_dir + '/projinfo.json') as f:
		return json.load(f)

def get_copyright(projinfo_copyright):
	ret = []
	for item in projinfo_copyright:
		years_str = []
		for year in sorted(item['years']):
			years_str.append(str(year))
		ret.append('© %s : %s'%(', '.join(years_str), item['author']))
	return ret

def compile(args):
	output = dict()
	timestamp = args['build_info']['start_time']
	if not 'build_host' in args['build_info']:
		args['build_info']['build_host'] = socket.gethostname()

	args['build_info']['start_time'] = datetime.datetime.fromtimestamp(timestamp, datetime.timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')
	output['build_info'] = buildinfo.substitute(args['build_info'])
	output['vcs_info'] = vcsinfo.substitute(get_vcsinfo(args['build_info']['target_dir']))
	projinfo = get_projinfo(args['build_info']['source_dir'])
	output['copyright'] = '\\n'.join(get_copyright(projinfo['copyright']))
	output['description_short'] = projinfo['description_short']
	output['legal_brief'] = projinfo['legal_info']['license_short']
	output_str = file_content.substitute(output)


	with open(args['targets'][0], 'w') as f:
		f.write(output_str)
	return 0

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))