# Maike

Maike is a build system based on scanning of source files. To construct the dependency graph, Maike uses embedded JSON data to control the compilation process. The best guide on how to tag source files and describe a project is probably the Maike source itself.

## Obtaining Maike

The source for the latest Maike can be obtained by cloning the git repository.

	git clone https://github.com/milasudril/maike

The latest release version can be found at [the github release page][1]. The former may contain features not present in the later, while the later should be used if there is no sufficiently recent version of Maike installed.

Before compiling maike, make sure that appropriate `libjansson` development files are available on the system. If Maike is not available, run the script `build.sh` within the source directory. It may happen that it is needed to change the permission of that file first.

	chmod u+x build.sh
	./build.sh

If Maike is available, just run your installed version of Maike from the source directory.

	maike

If the compilation succeeded, the maike executable should be present in the newly created `__targets` directory. In order to use it, copy it to a directory mentioned in your `PATH` variable. For example

	cp __targets/maike ~/bin/maike

## Creating a Maike project

Creating a Maike project is simple. Just create a project directory at an appropriate location in the file system and enter that directory.

	mkdir name_of_your_awesome_project
	cd name_of_your_awesome_project

Maike comes with a build-in default configuration that works with gcc 4.8 or later. To see the default configuration, issue the command

	maike --configdump --no-sysvars

If another configuration is desired, a configuration file can be generated using the default configuration as template

	maike --configdump=name_of_your_configuration --no-sysvars

or if I/O redirection is prefered

	maike --configdump --no-sysvars > name_of_your_configuration

It is possible to specify an additive  set of configuration files by using the option `--configfiles`. If this argument is not given, Maike will look for a file called `maikeconfig.json` within the current working directory. If that file cannot be openend, Maike will load a built-in default configuration.

## Adding targets to the project

Adding a target for use with Maike is simple. Just add a *target definition* to the corresponding source file, within the project directory. The "Hello, World" application looks like this:

	//@	{"targets":[{"name":"hello","type":"application"}]}

	#include <cstdio>

	int main()
		{
		puts("Hello, World");
		return 0;
		}

Running Maike on the project will now create an executable file called `hello` inside the target directory. The target directory is controlled by the key `targetinfo/target_directory` within the configuration file. Assuming it has been set to its default value, the program above can be executed by issuing the command

	__targets/hello

## An overview of the different forms of target definitions

Since Maike is a self-hosting application, a good reference on how to write various kinds of target defintions is the Maike source code. This introduction aims to explain common kinds of target defintions and their usecases,

### More than one translation unit

It is likely that your project consists of more than one *translation unit*. In that case, the linking is controlled through explicit *dependency attributes*. The main program may look like

	//@	{"targets":[{"name":"a_more_complicated_app","type":"application"}]}

	#include "dark_magic.hpp"
	#include <cstdio>

	int main()
		{
		try
			{
			DarkMagic::solveTheHaltingProblem();
			}
		catch(const char* message)
			{
			fprintf(stderr,"Error: %s\n",message);
			return 1;
			}
		return 0;
		}

The *include file* defines itself as an *include target*. It also adds extra dependencies to the target from which this file was included. In this case, `dark_magic.o` will become a dependency for `a_more_complicated_app`.

	//@	{
	//@	 "targets":[{"name":"dark_magic.hpp","type":"include"}]
	//@	,"dependencies_extra":[{"ref":"dark_magic.o","rel":"implementation"}]
	//@	}

	#ifndef DARKMAGIC_HPP
	#define DARKMAGIC_HPP

	namespace DarkMagic
		{
		void solveTheHaltingProblem();
		}

	#endif

Trying to compile the project without any definition of `dark_magic.o` will fail with a dependency error. Lets implement the `dark_magic` module.

	//@	{"targets":[{"name":"dark_magic.o","type":"object"}]}
	#include "dark_magic.hpp"

	void DarkMagic::solveTheHaltingProblem()
		{
		throw "Solving the halting problem is impossible";
		}

We can now recompile the project and there will be another executable file in the target directory. We can run the program by typing

	__targets/a_more_complicated_app

It may look like we have introduced a circular dependency between `dark_magic.hpp` and `dark_magic.o`. Maike avoids this by not adding any extra dependency that refers to the same target from which the file trying to add the dependency was included. **It is important to notice that Maike only follows *one* step back. Thus there will be a cyclic dependency if more steps are required to find the object target that included the current file.**


### Using external libraries

Sometimes it is convenient to rely on some external library. The include files for such libraries do not contain any useful linkage information, so in this case it is not enouth to just add the include files. In addition to adding the include file, a dependency is added directly. For example, a module that uses `libsndfile`--a library that decodes and encodes audio files--may look like

	//@	{
	//@	"targets":
	//@		[{
	//@		 "name":"lpfilter","type":"application"
	//@		,"dependencies":[{"ref":"sndfile","rel":"external"}]
	//@		}]
	//@	}

	#include <sndfile.h>
	#include <vector>

	float filterRun(float x_0,const float* buffer_in,float* buffer_out,size_t N)
		{
		while(N!=0)
			{
			auto x=*buffer_in;
			*buffer_out=0.125f*x + 0.875f*x_0;
			x_0=*buffer_out;
			++buffer_in;
			++buffer_out;
			--N;
			}
		return x_0;
		}

	int main(int argc,char** argv)
		{
		SF_INFO info;
		SNDFILE* file_in=sf_open(argv[1],SFM_READ,&info);
		if(info.channels!=1)
			{
			fprintf(stderr,"This program only supports mono files\n");
			sf_close(file_in);
			}

		SNDFILE* file_out=sf_open(argv[2],SFM_WRITE,&info);
		auto N=1024;
		auto n=N;
		std::vector<float> buffer_in(N*info.channels);
		std::vector<float> buffer_out(N*info.channels);
		auto x=0.0f;
		do
			{
			n=sf_readf_float(file_in,buffer_in.data(),N);
			x=filterRun(x,buffer_in.data(),buffer_out.data(),n);
			sf_writef_float(file_out,buffer_out.data(),n);
			}
		while(n==N);
		sf_close(file_in);
		sf_close(file_out);
		return 0;
		}

By placing the code that depends on the external library into a separate module, it is no longer neccesary to remember to link to `sndfile`. In the above scenario, we may add a module `sndfile_processor`. Using a simple C-style interface, the include file becomes

	//@	{
	//@	 "targets":[{"name":"sndfile_processor.h","type":"include"}]
	//@	,"dependencies_extra":[{"ref":"sndfile_processor.o","rel":"implementation"}]
	//@	}

	#ifndef SNDFILE_PROCESSOR_H
	#define SNDFILE_PROCESSOR_H

	#ifdef __cplusplus
	extern "C"
		{
	#endif
		typedef void (*ProcessCallback)(const float* buffer_in,float* buffer_out
			,size_t N,void* callback_param)

		void process(const char* file_in,const char* file_out,ProcessCallback callback
			,void* callback_param);

	#ifdef __cplusplus
		}
	#endif

	#endif

The dependency is now hidden inside the source for `sndfile_processor.o`:

	//@	{
	//@	"targets":
	//@		[{
	//@		 "name":"sndfile_processor.o","type":"object"
	//@		,"dependencies":[{"ref":"sndfile","rel":"external"}]
	//@		}]
	//@	}

	#include "sndfile_processor.hpp"
	#include <sndfile.h>
	#include <vector>

	void process(const char* file_in,const char* file_out,ProcessCallback callback
		,void* callback_param)
		{
	//	Do stuff that requires libsndfile, and call the callback function
	//	(which in this case will be the lowpass filter).
	//	...
		}

It may be interesting to find all external dependencies within the project. To do so, run Maike with the option `--list-external-targets`.

	maike --list-external-targets

### Conditional target selection

It may happen that parts of the code need to be specialized depending on the target platform. In this case, the target definition takes the form of a *case array*. A source file containing the definition below will only be active when compiling for a GNU/Linux system with kernel 2.6.33 or later:

	//@	[
	//@		[
	//@		"and(gnu,not(less_than(linux,version('2.6.33'))))"
	//@			,{
	//@			"targets":[{"name":"fileutils.o","type":"object"}]
	//@			}
	//@		]
	//@	]

It is possible to have more than one case. A source file that should be linked as a shared library may use this feature as follows:

	//@	[
	//@		[
	//@		"windows",
	//@			{"targets":[{"name":"your_library.dll","type":"lib_dynamic"}]}
	//@		],[
	//@		"os_x",
	//@			{"targets":[{"name":"your_library.dylib","type":"lib_dynamic"}]}
	//@		],{
	//@		"targets":
	//@			[{"name":"your_library.so","type":"lib_dynamic"}]
	//@		}
	//@	]

In this case, the first definition, whose condition evaluates to a non-zero value will be choosen. The condition is evaluated by using the variables set under the key `targetinfo` found in the configuration file. Normally, Maike examines the characteristics of the host system and sets these variables. This can be prevented by supplying the option `--no-sysvars` to Maike. This feature can be used when running a cross-compiler. Notice that when `--no-sysvars` is in effect, it is likely that you want to specify custom system variables in your configuration file.

### Using Python scripts as code generator

It is possible to insert Python scripts into the dependency graph. These scripts will be executed during the compilation process, and may produce arbitrary output. Such scripts can be used to dynamically capture version tag information from `git`.

	#@	{
	#@	"targets":
	#@		[
	#@			 {"name":"maikeinfo.cpp","dependencies":[]}
	#@			,{
	#@			 "name":"maikeinfo.o","type":"object","source_name":"maikeinfo.cpp"
	#@			,"source_generated":1
	#@			,"dependencies":[{"ref":"maikeinfo.hpp","rel":"internal"}]
	#@			}
	#@		]
	#@	}

	import sys
	import subprocess
	import time
	import string

	def write_error(*args, **kwargs):
		print(*args,file=sys.stderr,**kwargs)

	def gitVersionGet():
		with subprocess.Popen(('git', 'describe','--tags','--dirty','--always')\
			,stdout=subprocess.PIPE) as git:
			result=git.stdout.read().decode().strip()
			git.wait()
			status=git.returncode

		if status:
			with open('versioninfo.txt') as versionfile:
				result=versionfile.read().strip()
		else:
			with open('versioninfo.txt','w') as versionfile:
				versionfile.write(result)

		return result

	template=string.Template('''// This file has been generated by $scriptname

	#include "../maikeinfo.hpp"

	using namespace Maike;
	using namespace Info;

	const char* Info::VERSION="$version";
	const char* Info::TIMESTAMP="$now";
	''')

	try:
		filename_out=sys.argv[1]+'/maikeinfo.cpp'
		keys=dict()
		keys['scriptname']=sys.argv[0]
		keys['version']=gitVersionGet()
		keys['now']=time.strftime('%Y-%m-%d %H:%M')

		with open(filename_out,mode='w') as filehandle:
			filehandle.write(template.substitute(keys))
	except:
		write_error('%s: error: %s\n'%(sys.argv[0],sys.exc_info()[1]))
		sys.exit(-1)

A code generator defines at least two targets: The source file, and the target generated from the source file. The latter is neccesary, since the dependency graph has to be completed before the compilation starts. If the generator emmits N files, then there will be 2*N targets defined by that generator.


## TODO:s

See [Issues][2]

 [1]: https://github.com/milasudril/maike/releases/latest
 [2]: https://github.com/milasudril/maike/issues
