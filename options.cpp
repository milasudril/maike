//@	{"targets":[{"name":"options.o","type":"object"}]}

#include "options.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include "writebuffer.hpp"
#include <cstring>

using namespace Maike;

#define MAKE_OPTION(option_map,group,key,description,argcount) \
	option_map.get<Stringkey(key)>()={key,description,nullptr,argcount,group}

static void optionsFill(Options::OptionMap& options)
	{
	MAKE_OPTION(options,0,"help","Print this message to `stdout`, or to the given file, and exit.",1);

	MAKE_OPTION(options,0,"version","Print version information to `stdout`, or the given file, and exit.",1);

	MAKE_OPTION(options,1,"configfiles","Load the listed configuration files. "
		"If this argument is not given, Maike will look for a file called "
		"maikeconfig.json within the current working directory. If that file "
		"cannot be openend, Maike will load a built-in default configuration.",2);

	MAKE_OPTION(options,1,"no-sysvars","Do not load any default system variables. "
		"This is useful when using a cross-compiler. Normally, Maike loads a "
		"system-dependent set of variables that identifiesd the host platform.",0);

	MAKE_OPTION(options,1,"configdump","Print the current configuration to the "
		"given file, and exit. As defualt, the data is written to `stdout`.",1);

	MAKE_OPTION(options,2,"targets","Only process the listed targets. This "
		"option reduces the amount of output when using any of the --dump-* "
		"options.",2);

	MAKE_OPTION(options,2,"list-leaf-targets","Print all leaf targets to the "
		"given file and exit. As default, data is written to `stdout`. A leaf "
		"target is a target that no other target refers to.",1);

	MAKE_OPTION(options,2,"list-external-targets","Print all external targets to "
		"the given file and exit. As default, data is written to `stdout`. "
		"External targets are targets not tracked by Maike--usually external "
		"libraries.",1);

	MAKE_OPTION(options,2,"list-all-targets","Print all targets "
		"to the given file and exit. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(options,3,"dump-graph-dot","Dump the dependnecy graph to a DOT "
		"file and exit. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(options,3,"dump-graph-inv-dot","Dump an inverted dependnecy graph "
		"to a DOT file and exit. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(options,3,"dump-database-json","Dump the internal database to a JSON "
		"file and exit. As default, the data is written to `stdout`.",1);

	MAKE_OPTION(options,3,"dump-targets-tsv","Dumps information about the selected "
		"targets to a TSV (Tab Separated Values) file. As defualt, the data is "
		"written to `stdout`.",1);

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
						if(option_current->argcount<2 &&
							val_current->size()==option_current->argcount)
							{exceptionRaise(ErrorMessage("Command line error: Option #0; only takes 1 value.",{option_current->key}));}
						val_current->push_back(buffer);
						buffer.clear();
						break;
					case '\0':
						if(option_current->argcount<2 &&
							val_current->size()==option_current->argcount)
							{exceptionRaise(ErrorMessage("Command line error: Option #0; only takes 1 value.",{option_current->key}));}
						val_current->push_back(buffer);
						return;
					default:
						buffer+=ch_in;
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

void Options::printHelpImpl(DataSink& sink) const
	{
	WriteBuffer wb(sink);
	static constexpr size_t N_values=m_options.size();

	Option options_sorted[N_values];
	std::copy(m_options.values(), m_options.values() + N_values
		,options_sorted);
	std::sort(options_sorted,options_sorted + N_values,optionCompare);
	wb.write("Maike command line options\n")
		.write("==========================\n");
	auto i=options_sorted;
	auto i_end=options_sorted + N_values;
	while(i!=i_end)
		{
		wb.write("--").write(i->key)
			.write(i->argcount>0?((i->argcount>1)?"=string,...":"[=string]"):"")
			.write("\n    ").write(i->description).write("\n\n");
		++i;
		}
	}
