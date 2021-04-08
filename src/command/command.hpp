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
		explicit Literal(std::string&& val):m_value{std::move(val)}{}

		std::string const& value() const { return m_value; }

	private:
		std::string m_value;
	};

	CommandOutput expand(Literal const& obj);

	inline EvaluatedArgument makeEvaluatedArgument(Literal const& obj)
	{return EvaluatedArgument{obj.value()};}

	class ExpandString;

	class Command
	{
	public:
		std::string const& name() const { return m_name; }
		std::vector<std::variant<Literal, ExpandString>> args() const { return m_args; }

	private:
		std::string m_name;
		std::vector<std::variant<Literal, ExpandString>> m_args;
	};

	CommandOutput expand(Command const&);

	class CommandSplitOutput
	{
	public:
		Command const& command() const
		{ return m_command; }

		char separator() const
		{ return m_separator; }

	private:
		Command m_command;
		char m_separator;
	};

	EvaluatedArgument makeEvaluatedArgument(CommandSplitOutput const& obj);

	class ExpandString
	{
	public:
		Literal const& prefix() const { return m_prefix; }
		CommandSplitOutput command() const { return m_command; }
		Literal const& suffix() const { return m_suffix; }

	private:
		Literal m_prefix;
		CommandSplitOutput m_command;
		Literal m_suffix;
	};

	EvaluatedArgument makeEvaluatedArgument(ExpandString const& obj);

	class Pipe
	{
	public:
		auto const& commands() const { return m_commands; }
	private:
		std::vector<std::variant<Command, Literal>> m_commands;
	};

	CommandOutput expand(Pipe const& pipe);

#if 0
	struct SplitCommand
	{
		Command cmd;
		char separator;
	};

	struct VariableExpansion
	{
		CommandInput prefix;
		SplitCommand command;
		CommandInput suffix;
	};

	inline bool operator==(Command const&, Command const&)
	{
		return true;
	}

	inline bool operator!=(Command const& a, Command const& b)
	{
		return !(a == b);
	}


	std::vector<std::byte> execute(Command const& cmd, Invoker const& i);

	Command makeCommand(char const* str);
#endif
}