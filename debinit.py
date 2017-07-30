#!/usr/bin/python3

import sys
import os
import json
import string
import time
import shutil
import subprocess
import email.utils
import readline
import stat

changelog=string.Template('''$name_lower ($version$package_distro_suffix) $package_distro_release; urgency=low

  * Packaged for $package_distro
  
 -- $packager_name <$packager_email>  $package_date''')


copyright=string.Template('''Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: $name_lower
Source: $homepage

Files: *
Copyright: $years $author <$author_email>

License: $license_name

Files: debian/*
Copyright: $package_year $packager_name <$packager_email>
License: $license_name
$license_short
 .
 On Debian systems, you should find the complete license text in /usr/share/common-licenses/$license_file
''')

control=string.Template('''Source: $name_lower
Section: $debian_section
Priority: optional
Maintainer: $packager_name <$packager_email>
Build-Depends: debhelper (>=9),$build_deps
Standards-Version: 3.9.7
Homepage: $homepage
Vcs-Git: $vcs

Package: $name_lower
Architecture: any
Depends: $${shlibs:Depends}, $${misc:Depends}
Description: $description
 $description_long''')

compat='''9
'''

rules='''#!/usr/bin/make -f

%:
	dh $@'''

source_format='''3.0 (quilt)
'''


def write_error(*args, **kwargs):
    print(*args,file=sys.stderr,**kwargs)

def get_revision():
	if shutil.which('git')==None:
		with open('versioninfo-in.txt') as versionfile:
			result=versionfile.read().decode().strip()
	else:
		with subprocess.Popen(('git', 'describe','--tags','--dirty','--always') \
			,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL) as git:
			result=git.stdout.read().decode().strip()
			git.wait()
			status=git.returncode

		if status:
			with open('versioninfo-in.txt') as versionfile:
				result=versionfile.read().strip()
	return result


def load_json(filename):
	with open(filename,encoding='utf-8') as input:
		return json.load(input)
	
def write_file(filename,content):
	with open(filename,'wb') as f:
		f.write(content.encode('utf-8'))
		
def get(projinfo,caption,key):
	res=input(caption%projinfo[key]).strip()
	if res=='*':
		projinfo[key]=''
	if not res:
		print('    '+projinfo[key])
		return
	projinfo[key]=res
	print('    '+projinfo[key])
	
def dpkg_search(filename):
	if filename==None:
		return ''
	if shutil.which('dpkg')==None:
		return ''
	
	with subprocess.Popen(('dpkg', '--search',filename) \
		,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL) as dpkg:
		result=dpkg.stdout.read().decode().split('\n')
		dpkg.wait()
		status=dpkg.returncode
		if status:
			return ''
		return result[0].split(':')[0].strip()
	
	
	
def package_guess(kind,name):
	if kind=='dev files for':
		return dpkg_search(name+'.h')
	return dpkg_search(shutil.which(name))
	
def get_dep(kind,name):
	guess=package_guess(kind,name)
	res=input('    Enter the name of the package containing the %s `%s` (%s): '%(kind,name,guess)).strip()
	if res=='*':
		return ''
	if not res:
		print('      '+guess)
		return guess
	print('      '+res)
	return res
		
def get_build_deps(projinfo,deps):
	print('\nBuild dependencies (currently %s)\n. I will guess the corresponding packages based on the database of installed packages. Before accepting the guess, *make sure that it REALLY is correct*. If it is not, type the name of the correct package.'%projinfo['build_deps'])
	build_deps=[]
	for tool in deps['tools']:
		dep=get_dep('tool',tool)
		if dep!='':
			build_deps.append(dep)
			
	for lib in deps['libraries']:
		dep=get_dep('dev files for',lib)
		if dep!='':
			build_deps.append(dep)
	projinfo['build_deps']=','.join(build_deps)

try:
	projinfo=load_json('projectinfo.json')
	projinfo['version']=get_revision()
	projinfo['name_lower']=projinfo['name'].lower()
	now=time.time();
	projinfo['package_date']=email.utils.formatdate(now)
	projinfo['package_year']=time.strftime('%Y',time.gmtime(now))
	projinfo['packager_name']='John Doe'
	projinfo['packager_email']='john.doe@example.com'
	projinfo['package_distro']='Ubuntu'
	projinfo['package_distro_suffix']='-1a1'
	projinfo['package_distro_release']='xenial'
	projinfo['build_deps']=''
	projinfo['license_short']=' '+'\n .\n '.join(projinfo['license_short'].split('\n\n'))

	deps=load_json('externals.json')
	print('''\nBefore creating the debian directory, I need some information about this package. Leave blank to keep the default value. To answer with blank enter *.\n''')
		
	get(projinfo,'  Your name (%s): ','packager_name')
	get(projinfo,'  Your e-mail (%s): ','packager_email')
	get(projinfo,'  Target distribution (%s): ','package_distro')
	get(projinfo,'  Target distribution suffix (%s): ','package_distro_suffix')
	get(projinfo,'  Target distribution release (%s): ','package_distro_release')
	get_build_deps(projinfo,deps)
	
	print('''\nThis is the packaging information I have got:\n''')
	for k in ['packager_name','packager_email','package_distro'\
		,'package_distro_suffix','package_distro_release','build_deps']:
		print('  %s: %s'%(k,projinfo[k]))
	input('\nPress enter to build the package')

	try:
		shutil.rmtree('debian')
	except:
		pass

	os.mkdir('debian')

	write_file('debian/compat',compat)
	write_file('debian/copyright',copyright.substitute(projinfo))
	write_file('debian/changelog',changelog.substitute(projinfo))
	write_file('debian/control',control.substitute(projinfo))
	write_file('debian/rules',rules)
	st = os.stat('debian/rules') #debian/ruls should be executable
	os.chmod('debian/rules', st.st_mode | stat.S_IEXEC)
	os.mkdir('debian/source')
	write_file('debian/source/format',source_format)
except Exception:
	write_error('%s:%d: error: %s\n'%(sys.argv[0],sys.exc_info()[2].tb_lineno,sys.exc_info()[1]))
	sys.exit(-1)