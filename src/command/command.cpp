//@	{
//@	 "targets":[{"name": "command.o", "type":"object"}]
//@	}

#include "./command.hpp"

#include <algorithm>

Maike::CommandInterpreter::CommandOutput Maike::CommandInterpreter::expand(Literal const& obj)
{
	CommandOutput ret;
	auto const& val = obj.value();
	std::transform(std::begin(val), std::end(val), std::back_inserter(ret),
					[](auto val){return static_cast<std::byte>(val);});
	return ret;
}

Maike::CommandInterpreter::CommandOutput Maike::CommandInterpreter::expand(Command const& cmd)
{
	auto const& args = cmd.args();
	std::vector<std::string> args_eval;
	args_eval.reserve(std::size(args));
	std::for_each(std::begin(args), std::end(args), [&args_eval](auto const& arg){
		auto arg_eval = std::visit([](auto const& item){return makeEvaluatedArgument(item);}, arg);
		std::copy(std::begin(arg_eval), std::end(arg_eval), std::back_inserter(arg_eval));
	});
	// TODO:
	// return exep(cmd.executable(), args);
	return CommandOutput{};
}

Maike::CommandInterpreter::EvaluatedArgument Maike::CommandInterpreter::makeEvaluatedArgument(CommandSplitOutput const& obj)
{
	auto const command_output = expand(obj.command());

	EvaluatedArgument output_split;
	std::string buffer;
	auto ptr = std::begin(command_output);
	auto const separator = obj.separator();
	while(true)
	{
		auto ch_in = static_cast<char>(*ptr);
		++ptr;
		if(ch_in == separator)
		{
			output_split.push_back(std::move(buffer));
		}
		else
		{
			buffer += ch_in;
		}
	}
	if(std::size(buffer) != 0)
	{
		output_split.push_back(std::move(buffer));
	}

	return output_split;
}

Maike::CommandInterpreter::EvaluatedArgument Maike::CommandInterpreter::makeEvaluatedArgument(ExpandString const& arg)
{
	auto const cmd_output = makeEvaluatedArgument(arg.command());

	EvaluatedArgument ret;
	std::transform(std::begin(cmd_output), std::end(cmd_output), std::back_inserter(ret), [&prefix = arg.prefix().value(),
		&suffix = arg.suffix().value()
	](auto const& item) {
		return prefix + item + suffix;
	});

	return ret;
}

Maike::CommandInterpreter::CommandOutput Maike::CommandInterpreter::expand(Pipe const& pipe)
{
	auto const& commands = pipe.commands();
	CommandOutput ret;
	std::for_each(std::begin(commands), std::end(commands), [&ret](auto const& cmd) {
		ret = std::visit([](auto const& item){return expand(item);}, cmd);
	});

	return ret;
}


#if 0
Maike::CommandInterpreter::Command Maike::CommandInterpreter::makeCommand(char const* str)
{
	Command cmd;
	//	enum class State:int{Command, Escape, ArgList, State::BeforeCommandInput, State::CommandInput};
	//	auto state = State::Command;
	//	auto state_prev = state;
	//	std::string buffer;
	while(true)
	{
		auto ch_in = *str;
		if(ch_in == '\0') { return cmd; }
		if(ch_in == '~')
			{
			state_prev = state;
			state = State::Escape;
			continue;
		}

		switch(state)
		{
			case State::Command:
				switch(ch_in)
				{
					case '('
						cmd.name = std::move(buffer);
						state = State::ArgList;
						break;

					default:
						buffer += ch_in;
						break;
				}
				break;

			case State::ArgList:
				switch(ch_in)
				{
					case '(':
						push(cmd, state);
						cmd.name = std::move(buffer);
						state = State::Command;
						break;

					case ',':
						cmd.args.push_back(arg);
						break;

					case ')':
						state = State::BeforeCommandInput;
						break;

					default:
						buffer += ch_in;
				}

			case State::BeforeCommandInput:
				switch(ch_in)
				{
					case '<':
						state = CommandInput;
						break;
					default:
						[cmd, state] = pop();
				}

			case State::CommandInput:
				switch(ch_in)
				{
					case '(': {
						Command stdin;
						stdin.name = std::move(buffer);
						cmd.stdin = stdin;

						state = State::Command;
						break;
					}
					default:
						buffer += ch_in;
				}
				break;

			case State::Escape:
				buffer += ch_in;
				state = state_prev;
				break;
		}
	}
	return cmd;
}
#endif