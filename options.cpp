//@	{"targets":[{"name":"options.o","type":"object"}]}

#include "options.hpp"

using namespace Maike;

Options::Options(Twins<const char* const*> args)
	{
	m_options.get<Stringkey("help")>()
		={"Prints this message to `stdout`, or the given file",1};

	m_options.get<Stringkey("version")>()
		={"Prints version information to `stdout` or the given file",1};

	m_options.get<Stringkey("configfiles")>()
		={"Load the listed configuration files",2};

	m_options.get<Stringkey("no-stdconfig")>()
		={"Do not load any default configuration files",0};

	m_options.get<Stringkey("targets")>()
		={"Only process the listed targets. This options has no effect on any of "
			"the list* option",2};

	m_options.get<Stringkey("list-leaf-targets")>()
		={"Prints all leaf targets to the given file or `stdout` if no file is "
			"given. A leaf target is a target that no other target "
			"refers to. When this option is used, no target is processed.",1};

	m_options.get<Stringkey("list-external-targets")>()
		={"Prints all external targets to the given file or `stdout` if no file "
			"is given. External targets are targets not tracked by Maike--usually "
			"external libraries. When this option is used, no target is processed.",1};

	m_options.get<Stringkey("list-all-targets")>()
		={"Prints *all* targets to the given file or `stdout` if no file is "
			"given. When this option is used, no target is processed. When used "
			"together with any other list* option, this option takes preceedence",1};

	m_options.get<Stringkey("dump-graph")>()
		={"Dumps the dependnecy graph as a DOT file. As default, the data is "
			"written to `stdout`.",1};

	m_options.get<Stringkey("dump-target-usecount")>()
		={"Prints the number of references to all of the targets. As defualt, "
			"the data is written to `stdout`.",1};

	m_options.get<Stringkey("dump-timestat")>()
		={"Prints the time it takes to perform various tasks. As defualt, the "
			"data is written to `stdout`.",1};
	}

void Options::optionsPrint() const
	{
	auto i=m_options.begin();
	auto i_end=m_options.end();
	while(i!=i_end)
		{
		printf("%s\n\n",m_options[i].description);
		++i;
		}
	}
