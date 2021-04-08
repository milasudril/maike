//@	{
//@	 "targets":[{"name": "command.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#include "src/invoker.hpp"

#include <memory>
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

	class Literal
	{
	public:
	private:
		std::string m_value;
	};

	class ExpandString;

	class Command
	{
	private:
		std::string m_name;
		std::vector<std::variant<Literal, ExpandString>> m_args;
	};

	class CommandSplitOutput
	{
	public:
	private:
		Command m_command;
		char m_separator;
	};

	class ExpandString
	{
	private:
		Literal m_prefix;
		CommandSplitOutput m_command;
		Literal m_suffix;
	};

	class Pipe
	{
	public:
	private:
		std::vector<std::variant<Command, Literal>> m_commands;
	};
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