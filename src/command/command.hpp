//@	{
//@	 "targets":[{"name": "command.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#include "src/invoker.hpp"

#include <string>
#include <vector>
#include <variant>

namespace Maike::CommandInterpreter
{
	/* Basic syntax
	 *
	 *
	 * Use ~ as escape symbol
	 *
	 * system.cat(foo.cpp)|system.g++(-, -x, c++, -std=c++17, system.pkg-config(--libs,gtk+-3)/~ , -o,
	 * foo.o)
	 */

	using CommandOutput = std::vector<std::byte>;

	using EvaluatedArgument = std::vector<std::string>;

	class Literal
	{
	public:
		explicit Literal(std::string&& val): m_value{std::move(val)}
		{
		}

		std::string const& value() const
		{
			return m_value;
		}

	private:
		std::string m_value;
	};

	CommandOutput expand(Literal const& obj, CommandOutput const& sysin);

	inline EvaluatedArgument makeEvaluatedArgument(Literal const& obj)
	{
		return EvaluatedArgument{obj.value()};
	}

	class ExpandString;

	class Command
	{
	public:
		explicit Command(std::string&& name): m_name{std::move(name)}
		{
		}

		std::string const& name() const
		{
			return m_name;
		}
		std::vector<std::variant<Literal, ExpandString>> args() const
		{
			return m_args;
		}

		Command& add(Literal&& arg);

		Command& add(ExpandString&& str);

	private:
		std::string m_name;
		std::vector<std::variant<Literal, ExpandString>> m_args;
	};

	CommandOutput expand(Command const&, CommandOutput const& sysin);

	class Command;

	class Pipe
	{
	public:
		auto const& commands() const
		{
			return m_commands;
		}

		Pipe& operator|=(Command&& cmd)
		{
			m_commands.push_back(std::move(cmd));
			return *this;
		}

		Pipe& operator|=(Literal&& x)
		{
			m_commands.push_back(std::move(x));
			return *this;
		}

	private:
		std::vector<std::variant<Command, Literal>> m_commands;
	};

	inline Pipe& operator|(Pipe& a, Command&& cmd)
	{
		return a |= std::move(cmd);
	}

	inline Pipe& operator|(Pipe& a, Literal&& x)
	{
		return a |= std::move(x);
	}

	CommandOutput expand(Pipe const& pipe, CommandOutput const& sysin);


	class CommandSplitOutput
	{
	public:
		Pipe const& pipe() const
		{
			return m_pipe;
		}

		char separator() const
		{
			return m_separator;
		}

	private:
		Pipe m_pipe;
		char m_separator;
	};

	EvaluatedArgument makeEvaluatedArgument(CommandSplitOutput const& obj);

	class ExpandString
	{
	public:
		Literal const& prefix() const
		{
			return m_prefix;
		}
		CommandSplitOutput command() const
		{
			return m_command;
		}
		Literal const& suffix() const
		{
			return m_suffix;
		}

	private:
		Literal m_prefix;
		CommandSplitOutput m_command;
		Literal m_suffix;
	};

	inline Command& Command::add(Literal&& arg)
	{
		m_args.push_back(std::move(arg));
		return *this;
	}

	inline Command& Command::add(ExpandString&& str)
	{
		m_args.push_back(std::move(str));
		return *this;
	}

	EvaluatedArgument makeEvaluatedArgument(ExpandString const& obj);
}