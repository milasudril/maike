//@	 {"targets":[{"name":"command.o","type":"object"}]}

#include "command.hpp"
#include "variant.hpp"
#include "invoker.hpp"
#include "stringkey.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"

using namespace Maike;

static std::vector<const char*> cstr(Twins<const std::string*> strings)
	{
	std::vector<const char*> ret;
	while(strings.first!=strings.second)
		{
		ret.push_back(strings.first->c_str());
		++strings.first;
		}
	return std::move(ret);
	}

void Command::execute(Invoker& invoker,DataSink& standard_output,DataSink& standard_error)
	{
	auto args=cstr({m_args.data(),m_args.data() + m_args.size()});
	invoker.run(m_name.c_str()
		,{args.data(),args.data() + args.size()},standard_output,standard_error);
	}


static std::string varsSubstitute(const char* str
	,const std::map<Stringkey,std::string>& substitutes)
	{
	std::string ret;
	std::string var;
	enum class State:unsigned int{NORMAL,VARIABLE,ESCAPE};
	auto state=State::NORMAL;

	while(true)
		{
		auto ch_in=*str;
		switch(state)
			{
			case State::NORMAL:
				switch(ch_in)
					{
					case '$':
						state=State::VARIABLE;
						break;
					case '\\':
						state=State::ESCAPE;
						break;
					case '\0':
						return ret;
					default:
						ret+=ch_in;
					}

			case State::VARIABLE:
				if(ch_in>='\0' && ch_in<=' ')
					{
					auto i=substitutes.find(Stringkey(var.c_str()));
					if(i==substitutes.end())
						{exceptionRaise(ErrorMessage("Error executing command: The variable #0; is not defined.",{var.c_str()}));}
					ret+=i->second;
					if(ch_in=='\0')
						{return ret;}
					state=State::NORMAL;
					var.clear();
					}
				else
					{var+=ch_in;}
				break;

			case State::ESCAPE:
				ret+=ch_in;
				state=State::NORMAL;
				break;
			}
		++str;
		}
	}

void Command::execute(Invoker& invoker,DataSink& standard_output,DataSink& standard_error
	,const std::map<Stringkey,std::string>& substitutes)
	{
	std::vector<std::string> args_temp;
	auto ptr_args=m_args.data();
	auto ptr_end=ptr_args + m_args.size();

	while(ptr_args!=ptr_end)
		{
		args_temp.push_back(varsSubstitute(ptr_args->c_str(),substitutes));
		++ptr_args;
		}

	auto args=cstr({args_temp.data(),args_temp.data() + m_args.size()});
	invoker.run(m_name.c_str()
		,{args.data(),args.data() + args.size()},standard_output,standard_error);
	}

Command& Command::nameSet(const char* name)
	{
	m_name=std::string(name);
	return *this;
	}

Command& Command::argumentAppend(const char* arg)
	{
	m_args.push_back(arg);
	return *this;
	}
