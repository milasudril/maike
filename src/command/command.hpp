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
	* <VariableExpansion> ::= (<Value>? '{' <Command> '}' <Value>?) | <Command>
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

		private:
			std::unique_ptr<T> m_val;

	};

	class VariableExpansion
	{
		private:
			std::string m_prefix;
			std::string m_name;
			std::string m_suffix;
	};

	using Value = std::vector<std::string>;
	class Command;

	using Argument = std::variant<Value, VariableExpansion, RecursiveWrapper<Command>>;

	class Command
	{
		private:
			Argument m_stdin;
			std::string m_name;
			std::vector<Argument> m_args;
	};
}