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
	* <Command> ::= [<Argument>] '|'] <command_id> '(' <arglist> ')' [ '/' <Separator> ]
	* <arglist> ::= '' | <Argument> (',' <Argument>)*
	* <Argument> ::= <Value> | <VariableExpansion> | <Command>
	* <VariableExpansion> ::= <Value>? '{' <Command> '}' <Value>?
	* <command_id> :: <namespace> '.' <name>
	*
	* Use ! as escape symbol
	*
	*/
	template<class T>
	class RecursiveWrapper
	{
		public:
			template<class ... Args>
			RecursiveWrapper(Args&& ... args): m_val{std::make_unique<Args...>(std::forward<Args>(args)...)}{}

			operator const T&() const { return *m_val; }

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


	using Value = std::vector<std::string>;
	class VariableExpansion;

	using Argument = std::variant<Value, RecursiveWrapper<VariableExpansion>>;

	struct Command
	{
		Argument stdin;
		std::string name;
		std::vector<Argument> args;
	};

	struct VariableExpansion
	{
		std::string prefix;
		Command command;
		std::string suffix;
	};

	inline bool operator==(Command const& a, Command const& b)
	{
		return a.stdin == b.stdin && a.name == b.name && a.args == b.args;
	}

	inline bool operator!=(Command const& a, Command const& b)
	{
		return !(a == b);
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