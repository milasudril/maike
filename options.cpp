//@	{"targets":[{"name":"options.o","type":"object"}]}

#include "options.hpp"

using namespace Maike;

#define MAKE_OPTION(option_map,key,description,argcount) \
	option_map.get<Stringkey(key)>()={key,description,argcount}

Options::Options(Twins<const char* const*> args)
	{
	MAKE_OPTION(m_options,"help","Prints this message to `stdout`, or to the given file",1);

	MAKE_OPTION(m_options,"version","Prints version information to `stdout` or the given file",1);

	MAKE_OPTION(m_options,"configfiles","Load the listed configuration files",2);

	MAKE_OPTION(m_options,"no-stdconfig","Do not load any default configuration files",0);

	MAKE_OPTION(m_options,"targets","Only process the listed targets. This "
		"options has no effect on any of the list* option",2);

	MAKE_OPTION(m_options,"list-leaf-targets","Prints all leaf targets to the "
		"given file or `stdout` if no file is given. A leaf target is a target "
		"that no other target refers to. When this option is used, no target is "
		"processed.",1);

	MAKE_OPTION(m_options,"list-external-targets","Prints all external targets "
		"to the given file or `stdout` if no file is given. External targets "
		"are targets not tracked by Maike--usually external libraries. When "
		"this option is used, no target is processed.",1);

	MAKE_OPTION(m_options,"list-all-targets","Prints *all* targets to the given "
		"file or `stdout` if no file is given. When this option is used, no "
		"target is processed. When used together with any other list* option, "
		"this option takes preceedence",1);

	MAKE_OPTION(m_options,"dump-graph","Dumps the dependnecy graph as a DOT "
		"file. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(m_options,"dump-target-usecount","Prints the number of references to all "
		"of the targets. As defualt, the data is written to `stdout`.",1);

	MAKE_OPTION(m_options,"dump-timestat","Prints the time it takes to perform various "
		"tasks. As defualt, the data is written to `stdout`.",1);
	}

void Options::optionsPrint() const
	{
	auto i=m_options.begin();
	auto i_end=m_options.end();
	while(i!=i_end)
		{
		printf("--%s%s\n    %s\n\n",m_options[i].key
			,m_options[i].argcount>0?
				((m_options[i].argcount>1)?"=string,...":"[=string]"):""
			,m_options[i].description);
		++i;
		}
	}
