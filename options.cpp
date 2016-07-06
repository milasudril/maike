//@	{"targets":[{"name":"options.o","type":"object"}]}

#include "options.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include <cstring>

using namespace Maike;

#define MAKE_OPTION(option_map,group,key,description,argcount) \
	option_map.get<Stringkey(key)>()={key,description,argcount,group}

static void optionsFill(Options::OptionMap& options)
	{
	MAKE_OPTION(options,0,"help","Prints this message to `stdout`, or to the given file",1);

	MAKE_OPTION(options,0,"version","Prints version information to `stdout` or the given file",1);

	MAKE_OPTION(options,1,"targets","Only process the listed targets. This "
		"options has no effect on any of the list* option",2);

	MAKE_OPTION(options,2,"configfiles","Load the listed configuration files",2);

	MAKE_OPTION(options,2,"no-stdconfig","Do not load any default configuration files",0);

	MAKE_OPTION(options,2,"no-sysvars","Do not load any default system variable. "
		"This is useful when using a cross-compiler.",0);

	MAKE_OPTION(options,3,"list-leaf-targets","Prints all leaf targets to the "
		"given file or `stdout` if no file is given. A leaf target is a target "
		"that no other target refers to. When this option is used, no target is "
		"processed.",1);

	MAKE_OPTION(options,3,"list-external-targets","Prints all external targets "
		"to the given file or `stdout` if no file is given. External targets "
		"are targets not tracked by Maike--usually external libraries. When "
		"this option is used, no target is processed.",1);

	MAKE_OPTION(options,3,"list-all-targets","Prints *all* targets to the given "
		"file or `stdout` if no file is given. When this option is used, no "
		"target is processed. When used together with any other list* option, "
		"this option takes preceedence",1);

	MAKE_OPTION(options,4,"dump-graph","Dumps the dependnecy graph as a DOT "
		"file. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(options,4,"dump-target-usecount","Prints the number of references to all "
		"of the targets. As defualt, the data is written to `stdout`.",1);

	MAKE_OPTION(options,4,"dump-timestat","Prints the time it takes to perform various "
		"tasks. As defualt, the data is written to `stdout`.",1);
	}

static void optionLoad(Options::OptionMap& options
	,std::vector<std::string>* strings,const char* arg)
	{
	std::string buffer;
	enum class State:unsigned int{START_0,START_1,KEY,VALUE,VALUE_ESCAPE};
	auto state=State::START_0;
	Options::Option* option_current=nullptr;
	std::vector<std::string>* val_current=nullptr;
	while(true)
		{
		auto ch_in=*arg;
		switch(state)
			{
			case State::START_0:
				{
				switch(ch_in)
					{
					case '-':
						state=State::START_1;
						break;
					default:
						exceptionRaise(ErrorMessage("Command line error: Expected `-` got #0;.",{ch_in}));
					}
				}
				break;

			case State::START_1:
				{
				switch(ch_in)
					{
					case '-':
						state=State::KEY;
						break;
					default:
						exceptionRaise(ErrorMessage("Command line error: Expected `-` got #0;.",{ch_in}));
					}
				}
				break;

			case State::KEY:
				{
				switch(ch_in)
					{
					case '=':
						{
						auto i=options.find(Stringkey(buffer.c_str()));
						if(i==options.end())
							{exceptionRaise(ErrorMessage("Command line error: Unknown option #0;.",{buffer.c_str()}));}
						option_current=&options[i];
						if(option_current->argcount==0)
							{exceptionRaise(ErrorMessage("Command line error: Option #0; does not take any arguments.",{buffer.c_str()}));}

						val_current=&strings[static_cast<size_t>(i)];
						option_current->values=val_current;
						buffer.clear();
						state=State::VALUE;
						}
						break;
					case '\0':
						{
						auto i=options.find(Stringkey(buffer.c_str()));
						if(i==options.end())
							{exceptionRaise(ErrorMessage("Command line error: Unknown option #0;.",{buffer.c_str()}));}
						option_current=&options[i];
						if(option_current->argcount>1)
							{exceptionRaise(ErrorMessage("Command line error: Option #0; requires at least 1 value.",{buffer.c_str()}));}
						val_current=&strings[static_cast<size_t>(i)];
						option_current->values=val_current;
						return;
						}

					default:
						buffer+=ch_in;
					}
				}
				break;

			case State::VALUE:
				switch(ch_in)
					{
					case '\\':
						state=State::VALUE_ESCAPE;
						break;
					case ',':
						val_current->push_back(buffer);
						buffer.clear();
						break;
					case '\0':
						val_current->push_back(buffer);
						return;
					}
				break;

			case State::VALUE_ESCAPE:
				if(ch_in=='\0')
					{exceptionRaise(ErrorMessage("Command line error: Lonely escape character.",{}));}
				state=State::VALUE;
				break;
			}
		++arg;
		}
	}

Options::Options(Twins<const char* const*> args)
	{
	optionsFill(m_options);
	if(args.first!=args.second)
		{++args.first;}
	while(args.first!=args.second)
		{
		optionLoad(m_options,m_data,*args.first);
		++args.first;
		}
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
	std::sort(options_sorted,options_sorted + N_values,optionCompare);

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
