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

		private:
			std::unique_ptr<T> m_val;

	};


	using Value = std::vector<std::string>;
	class VariableExpansion;

	using Argument = std::variant<Value, RecursiveWrapper<VariableExpansion>>;

	class Command
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
}