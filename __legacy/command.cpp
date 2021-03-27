//@	 {"targets":[{"name":"command.o","type":"object"}]}

#include "command.hpp"
#include "variant.hpp"
#include "stringkey.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "resourceobject.hpp"
#include "pipe.hpp"
#include "parameterset.hpp"

using namespace Maike;

Command::Command()
{
}

Command::Command(const ResourceObject& cmd)
{
	m_name = std::string(static_cast<const char*>(cmd.objectGet("name")));
	auto args = cmd.objectGet("args");
	auto N_args = args.objectCountGet();
	for(decltype(N_args) k = 0; k < N_args; ++k)
	{
		m_args.push_back(std::string(static_cast<const char*>(args.objectGet(k))));
	}
}

static std::vector<const char*> cstr(Twins<const std::string*> strings)
{
	std::vector<const char*> ret;
	while(strings.first != strings.second)
	{
		ret.push_back(strings.first->c_str());
		++strings.first;
	}
	return std::move(ret);
}

Pipe Command::execute(unsigned int redirection) const
{
	auto args = cstr({m_args.data(), m_args.data() + m_args.size()});
	return Pipe(m_name.c_str(), {args.data(), args.data() + args.size()}, redirection);
}

namespace
{
	class ParamExtractor: public ParameterSet::ParameterProcessor
	{
	public:
		ParamExtractor(const std::string& prefix, std::vector<std::string>& result):
		   r_prefix(prefix), r_result(result)
		{
		}
		void operator()(const char* value)
		{
			r_result.push_back(r_prefix + value);
		}

	private:
		const std::string& r_prefix;
		std::vector<std::string>& r_result;
	};
}

static void substitutesAppend(const std::string& prefix,
                              const Stringkey& key,
                              Twins<const ParameterSet* const*> substitutes,
                              std::vector<std::string>& result)
{
	auto n_0 = result.size();
	while(substitutes.first != substitutes.second)
	{
		--substitutes.second;
		(*substitutes.second)->parameterGet(key, ParamExtractor(prefix, result));
		auto n = result.size();
		if(n_0 != n) //	Parameter found
		{ return; }
		n_0 = n;
	}
}

static void varsSubstitute(const char* str,
                           Twins<const ParameterSet* const*> substitutes,
                           std::vector<std::string>& result)
{
	std::string temp;
	std::string name_old;
	enum class State : unsigned int
	{
		NORMAL,
		VARIABLE,
		ESCAPE
	};
	auto state = State::NORMAL;
	while(true)
	{
		auto ch_in = *str;
		switch(state)
		{
			case State::NORMAL:
				switch(ch_in)
				{
					case '{':
						name_old = temp;
						temp.clear();
						state = State::VARIABLE;
						break;

					case '\\': state = State::ESCAPE; break;

					case '\0':
						if(temp.size() != 0) { result.push_back(temp); }
						return;

					default: temp += ch_in;
				}
				break;

			case State::VARIABLE:
				switch(ch_in)
				{
					case '}':
						//	FIXME: How to treat fixed content together with array argument
						state = State::NORMAL;
						substitutesAppend(name_old, Stringkey(temp.c_str()), substitutes, result);
						name_old.clear();
						temp.clear();
						break;

					case '\0': exceptionRaise(ErrorMessage("Expansion error: Unterminated variable.", {})); break;

					default: temp += ch_in;
				}
				break;

			case State::ESCAPE:
				switch(ch_in)
				{
					case '\0':
						exceptionRaise(ErrorMessage("Expansion error: Lonely escape character.", {}));
						break;
					default: temp += ch_in; state = State::NORMAL;
				}
				break;
		}
		++str;
	}
}

Pipe Command::execute(unsigned int redirection, Twins<const ParameterSet* const*> substitutes) const
{
	std::vector<std::string> args_temp;
	auto ptr_args = m_args.data();
	auto ptr_end = ptr_args + m_args.size();

	while(ptr_args != ptr_end)
	{
		varsSubstitute(ptr_args->c_str(), substitutes, args_temp);
		++ptr_args;
	}

	auto args = cstr({args_temp.data(), args_temp.data() + args_temp.size()});
	return Pipe(m_name.c_str(), {args.data(), args.data() + args.size()}, redirection);
}

Command& Command::nameSet(const char* name)
{
	m_name = std::string(name);
	return *this;
}

Command& Command::argumentAppend(const char* arg)
{
	m_args.push_back(arg);
	return *this;
}

void Command::argumentsClear() noexcept
{
	m_args.clear();
}

void Command::configDump(ResourceObject& cmd) const
{
	cmd.objectSet("name", cmd.create(m_name.c_str()));

	{
		auto args = cmd.createArray();
		auto ptr_args = m_args.data();
		auto ptr_end = ptr_args + m_args.size();
		while(ptr_args != ptr_end)
		{
			args.objectAppend(args.create(ptr_args->c_str()));
			++ptr_args;
		}
		cmd.objectSet("args", std::move(args));
	}
}
