//@	{"targets":[{"name":"options.o","type":"object"}]}

#include "options.hpp"
#include <cstring>

using namespace Maike;

#define MAKE_OPTION(option_map,group,key,description,argcount) \
	option_map.get<Stringkey(key)>()={key,description,argcount,group}

Options::Options(Twins<const char* const*> args)
	{
	MAKE_OPTION(m_options,0,"help","Prints this message to `stdout`, or to the given file",1);

	MAKE_OPTION(m_options,0,"version","Prints version information to `stdout` or the given file",1);

	MAKE_OPTION(m_options,1,"targets","Only process the listed targets. This "
		"options has no effect on any of the list* option",2);

	MAKE_OPTION(m_options,2,"configfiles","Load the listed configuration files",2);

	MAKE_OPTION(m_options,2,"no-stdconfig","Do not load any default configuration files",0);

	MAKE_OPTION(m_options,3,"list-leaf-targets","Prints all leaf targets to the "
		"given file or `stdout` if no file is given. A leaf target is a target "
		"that no other target refers to. When this option is used, no target is "
		"processed.",1);

	MAKE_OPTION(m_options,3,"list-external-targets","Prints all external targets "
		"to the given file or `stdout` if no file is given. External targets "
		"are targets not tracked by Maike--usually external libraries. When "
		"this option is used, no target is processed.",1);

	MAKE_OPTION(m_options,3,"list-all-targets","Prints *all* targets to the given "
		"file or `stdout` if no file is given. When this option is used, no "
		"target is processed. When used together with any other list* option, "
		"this option takes preceedence",1);

	MAKE_OPTION(m_options,4,"dump-graph","Dumps the dependnecy graph as a DOT "
		"file. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(m_options,4,"dump-target-usecount","Prints the number of references to all "
		"of the targets. As defualt, the data is written to `stdout`.",1);

	MAKE_OPTION(m_options,4,"dump-timestat","Prints the time it takes to perform various "
		"tasks. As defualt, the data is written to `stdout`.",1);
	}

static bool optionCompare(const Options::Option& a,const Options::Option& b)
	{
	if(a.group < b.group)
		{return 1;}
	if(a.group==b.group && strcmp(a.key,b.key)<0)
		{return 1;}
	return 0;
	}

void Options::optionsPrint() const
	{
	static constexpr size_t N_values=m_options.size();

	Option options_sorted[N_values];
	std::copy(m_options.values(), m_options.values() + N_values
		,options_sorted);
	std::sort(options_sorted,options_sorted +N_values,optionCompare);

	auto i=options_sorted;
	auto i_end=options_sorted + N_values;
	while(i!=i_end)
		{
		printf("--%s%s\n    %s\n\n",i->key
			,i->argcount>0?
				((i->argcount>1)?"=string,...":"[=string]"):""
			,i->description);
		++i;
		}
	}
