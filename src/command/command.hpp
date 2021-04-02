//@	{
//@	 "targets":[{"name": "command.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace Maike::CommandInterpreter
{
	/* Basic syntax
	 *
	 * <Command> ::= <CommandName> '(' <ArgList> ')' [ '<' <CommandInput> ]
	 * <CommandInput> ::= <Value> | <Command>
	 * <CommandName> ::= <String>
	 * <String> ::= #'.*'
	 * <Value> ::= <String>
	 * <ArgList> ::= '' | <Argument> (',' <Argument>)*
	 * <Argument> ::= <ValueArray> | <VariableExpansion> | <SplitCommand>
	 * <ValueArray> ::= '' | (',' <Value>)*
	 * <VariableExpansion> ::= <CommandInput>? '{' <SplitCommand> '}' <CommandInput>?
	 * <SplitCommand> ::= <Command> '/' <Separator>
	 * <Separator> ::= #'.*'
	 *
	 * Use ~ as escape symbol
	 *
	 * system.g++(-, -x, c++, -std=c++17, system.pkg-config(--libs,gtk+-3)/~ , -o, foo.o) <
	 * cat(foo.txt)
	 *
	 */
	template<class T>
	class RecursiveWrapper
	{
	public:
		template<class... Args>
		RecursiveWrapper(Args&&... args): m_val{std::make_unique<Args...>(std::forward<Args>(args)...)}
		{
		}

		operator const T&() const
		{
			return *m_val;
		}

		bool operator==(const RecursiveWrapper& other) const
		{
			return *m_val == *other.m_val;
		}

		bool operator!=(const RecursiveWrapper& other) const
		{
			return !(*this == other);
		}

	private:
		std::unique_ptr<T> m_val;
	};


	using Value = std::string;
	using CommandName = std::string;
	class Command;
	using CommandInput = std::variant<Value, RecursiveWrapper<Command>>;
	class VariableExpansion;
	using Argument = std::variant<Value, RecursiveWrapper<VariableExpansion>>;
	using ArgList = std::vector<Argument>;

	struct Command
	{
		CommandInput stdin;
		CommandName name;
		ArgList args;
	};

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

	inline bool operator==(Command const& a, Command const& b)
	{
		return a.stdin == b.stdin && a.name == b.name && a.args == b.args;
	}

	inline bool operator!=(Command const& a, Command const& b)
	{
		return !(a == b);
	}


	inline bool operator==(SplitCommand const& a, SplitCommand const& b)
	{
		return a.cmd == b.cmd && a.separator == b.separator;
	}


	inline bool operator==(VariableExpansion const& a, VariableExpansion const& b)
	{
		return a.prefix == b.prefix && a.command == b.command && a.suffix == b.suffix;
	}

	inline bool operator!=(VariableExpansion const& a, VariableExpansion const& b)
	{
		return !(a == b);
	}

	Command makeCommand(char const* str);
}