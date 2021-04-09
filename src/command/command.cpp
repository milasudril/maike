//@	{
//@	 "targets":[{"name": "command.o", "type":"object"}]
//@	}

#include "./command.hpp"

#include <algorithm>
#include <stack>

Maike::CommandInterpreter::CommandOutput Maike::CommandInterpreter::expand(Literal const& obj,
                                                                           CommandOutput const&)
{
	CommandOutput ret;
	auto const& val = obj.value();
	std::transform(std::begin(val), std::end(val), std::back_inserter(ret), [](auto val) {
		return static_cast<std::byte>(val);
	});
	return ret;
}

Maike::CommandInterpreter::CommandOutput Maike::CommandInterpreter::expand(Command const& cmd,
                                                                           CommandOutput const&)
{
	auto const& args = cmd.args();
	std::vector<std::string> args_eval;
	args_eval.reserve(std::size(args));
	std::for_each(std::begin(args), std::end(args), [&args_eval](auto const& arg) {
		auto arg_eval = std::visit([](auto const& item) { return makeEvaluatedArgument(item); }, arg);
		std::copy(std::begin(arg_eval), std::end(arg_eval), std::back_inserter(arg_eval));
	});
	// TODO:
	// return exep(cmd.executable(), args, sysín);
	return CommandOutput{};
}

Maike::CommandInterpreter::EvaluatedArgument
Maike::CommandInterpreter::makeEvaluatedArgument(CommandSplitOutput const& obj)
{
	auto const command_output = expand(obj.pipe(), CommandOutput{});

	EvaluatedArgument output_split;
	std::string buffer;
	auto ptr = std::begin(command_output);
	auto const separator = obj.separator();
	while(true)
	{
		auto ch_in = static_cast<char>(*ptr);
		++ptr;
		if(ch_in == separator) { output_split.push_back(std::move(buffer)); }
		else
		{
			buffer += ch_in;
		}
	}
	if(std::size(buffer) != 0) { output_split.push_back(std::move(buffer)); }

	return output_split;
}

Maike::CommandInterpreter::EvaluatedArgument
Maike::CommandInterpreter::makeEvaluatedArgument(ExpandString const& arg)
{
	auto const cmd_output = makeEvaluatedArgument(arg.command());

	EvaluatedArgument ret;
	std::transform(std::begin(cmd_output),
	               std::end(cmd_output),
	               std::back_inserter(ret),
	               [&prefix = arg.prefix().value(), &suffix = arg.suffix().value()](auto const& item) {
		               return prefix + item + suffix;
	               });

	return ret;
}

Maike::CommandInterpreter::CommandOutput
Maike::CommandInterpreter::expand(Pipe const& pipe, CommandOutput const& sysin)
{
	auto const& commands = pipe.commands();
	CommandOutput sysout{sysin};
	std::for_each(std::begin(commands), std::end(commands), [&sysout](auto const& cmd) {
		sysout = std::visit([&sysin = sysout](auto const& item) { return expand(item, sysin); }, cmd);
	});

	return sysout;
}


Maike::CommandInterpreter::Pipe Maike::CommandInterpreter::makePipe(char const* str)
{
	enum class State : int
	{
		Init,
		ArgList,
		ExpandString,
		AfterExpandString,
		AfterCommand,
		Escape
	};

	struct Context
	{
		Pipe node;
		State state{State::Init};
		State state_prev{State::Init};
		std::string buffer;
		int bracecount{0};
	};

	std::stack<Context> contexts;
	Context ctxt;
	while(true)
	{
		auto ch_in = *str;
		++str;

		if(ch_in == '\0')
		{
			if(std::size(contexts) != 0) { throw std::runtime_error{"Unterminated command"}; }

			return ctxt.node;
		}

		if(ch_in == '~')
		{
			ctxt.state_prev = ctxt.state;
			ctxt.state = State::Escape;
			continue;
		}

		if(ctxt.state != State::Escape)
		{
			if(ch_in >= 0 && ch_in <= ' ') { continue; }
		}

		switch(ctxt.state)
		{
			case State::Init:
				switch(ch_in)
				{
					case '|': ctxt.node |= Literal{std::move(ctxt.buffer)}; break;

					case '(':
						ctxt.node |= Command{std::move(ctxt.buffer)};
						ctxt.state = State::ArgList;
						break;

					default: ctxt.buffer += ch_in;
				}
				break;


			case State::ArgList:
			{
				auto& cmd = *std::get_if<Command>(&ctxt.node.back());
				switch(ch_in)
				{
					case '{':
						cmd.add(ExpandString{Literal{std::move(ctxt.buffer)}});
						ctxt.state = State::ExpandString;
						++ctxt.bracecount;
						break;

					case ',': cmd.add(Literal{std::move(ctxt.buffer)}); break;

					case ')':
						cmd.add(Literal{std::move(ctxt.buffer)});
						ctxt.state = State::AfterCommand;
						break;

					default: ctxt.buffer += ch_in;
				}
				break;
			}

			case State::ExpandString:
				switch(ch_in)
				{
					case '{': ++ctxt.bracecount; break;
					case '}':
						--ctxt.bracecount;
						if(ctxt.bracecount == 0) { ctxt.state = State::AfterExpandString; }
						break;
					default: break;
				};
				break;

			case State::AfterExpandString:
			{
				auto& cmd = *std::get_if<Command>(&ctxt.node.back());
				auto& expand_string = *std::get_if<ExpandString>(&cmd.back());
				switch(ch_in)
				{
					case ')':
						expand_string.suffix(Literal{std::move(ctxt.buffer)});
						ctxt.state = State::Init;
						break;

					case ',':
						expand_string.suffix(Literal{std::move(ctxt.buffer)});
						ctxt.state = State::ArgList;
						break;

					default: ctxt.buffer += ch_in;
				}
				break;
			}
			case State::AfterCommand:
				switch(ch_in)
				{
					case '|': ctxt.state = State::Init; break;

					case ',': ctxt.state = State::ArgList; break;

					default: puts(str); throw std::runtime_error{"Illegal character after command"};
				}
				break;


			case State::Escape:
				ctxt.buffer += ch_in;
				ctxt.state = ctxt.state_prev;
				break;
		}
	}

	return ctxt.node;
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