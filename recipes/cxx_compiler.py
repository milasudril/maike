#!/usr/bin/env python3

#@	{"dependencies":[{"ref":"pkg_config.py","origin":"project"}]}

import sys
import json
import os
import subprocess
import shutil
import pkg_config

compiler = os.environ['CXX'] if 'CXX' in os.environ else 'g++'

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def format_iquote(src_dir, list):
	ret = []
	for item in list:
		if src_dir == '.':
			ret.append('-iquote%s'%(item))
		else:
			ret.append('-iquote%s/%s'%(src_dir, item))
	return ret

def collect_cflags(src_dir, compiler_flags, dependencies):
	ret = set()
	ret.update(compiler_flags['cflags'])
	ret.update(format_iquote(src_dir, compiler_flags['iquote']))
	for item in dependencies:
		if item['origin'] == 'pkg-config':
			ret.update(pkg_config.get_cflags(item['ref']))
	ret.add('-std=c++17')
	return ret

no_op = {'.hpp', '.h', '.hxx'}

def compile(build_args):
	ext = os.path.splitext(build_args['source_file'].lower())
	if ext[1] in no_op:
		return 0
	args = []
	args.append(compiler)
	args.extend(collect_cflags(build_args['build_info']['source_dir'], build_args['compiler_cfg'], build_args['dependencies']))
	if 'std_revision' in build_args['compiler_cfg']:
		if 'selected' in build_args['compiler_cfg']['std_revision']:
			args.append('-std=%s'%build_args['compiler_cfg']['std_revision']['selected'])
	args.append('-fdiagnostics-color=%s'%('always' if build_args['log_format']=='ansi_term' else 'never'))
	args.append('-c')
	args.append(build_args['source_file'])
	args.append('-o')
	args.append(build_args['targets'][0])
	result = subprocess.run(args)

	for target in build_args['targets'][1:]:
		shutil.copyfile(build_args['targets'][0], target)

	return result.returncode


def get_numeric_rev(rev):
	rev_constants = {'c++98': 199711, 'c++11': 201103, 'c++14': 201402, 'c++17': 201703, 'c++17': 201702, 'c++20': 202002}
	return rev_constants[rev]

def get_cpp_revision_tag(std = None):
	args = []
	if std == None:
		args = [compiler, '-x', 'c++', '-E', '-dM', '/dev/null']
	else:
		args = [compiler, '-x', 'c++', '-std=%s'%std, '-E', '-dM', '/dev/null']
	result = subprocess.run(args, stdin = subprocess.DEVNULL, stdout=subprocess.PIPE, text=True)
	if result.returncode != 0:
		raise Exception('The requested revision %s appears to be unsupported by the selected compiler %s'%(std, compiler))

	for line in result.stdout.split('\n'):
		fields=line.split()
		if len(fields) >= 3:
			if fields[1] == '__cplusplus':
				return int(fields[2][0:-1])

def select_cpp_rev(rev):
	if 'min' in rev:
		min_num = get_numeric_rev(rev['min'])
		if 'max' in rev:
			max_num = get_numeric_rev(rev['max'])
			if min_num > max_num:
				raise Exception('Impossible configuration (min %s is newer than max %s)'%(rev['min'], rev['max']))
			if rev['min'] == rev['max']:
				return rev['min']
			else:
				default_rev = get_cpp_revision_tag()
				if default_rev < min_num:
					return rev['min']
				if default_rev > max_num:
					return rev['max']
				return ''
		else:
			default_rev = get_cpp_revision_tag()
			if default_rev < min_num:
				return rev['min']
	else:
		if 'max' in rev:
			default_rev = get_cpp_revision_tag()
			if default_rev > max_num:
				return rev['max']

def configure(cfg):
	if 'std_revision' in cfg:
		ret = select_cpp_rev(cfg['std_revision'])
		if ret != '':
			rev_cfg = get_cpp_revision_tag(ret)
			if rev_cfg < get_numeric_rev(ret):
				eprint('Warning: The compiler reports an earlier standard revision than requested (%d vs %d). This indicates that support for the selected revision is experimental. Expect changes in ABI or API when the compiler is upgraded.'%(rev_cfg, get_numeric_rev(ret)))
			cfg['std_revision']['selected'] = ret
		print(json.dumps(cfg))
		return 0
	else:
		return 0


if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		exit(compile(json.loads(sys.argv[2])))

	if sys.argv[1] == 'configure':
		exit(configure(json.loads(sys.argv[2])))
