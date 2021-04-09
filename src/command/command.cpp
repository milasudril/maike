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

bool Maike::CommandInterpreter::operator==(Command const& a, Command const& b)
{
	return a.name() == b.name() && a.args() == b.args();
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


std::pair<Maike::CommandInterpreter::Pipe, char const*>
Maike::CommandInterpreter::makePipe(char const* str)
{
	enum class State : int
	{
		Init,
		ArgList,
		ExpandStringFirstCommand,
		AfterExpandStringPipe,
		Separator,
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

			return std::pair{ctxt.node, str - 1};
		}

		if(ch_in == '~')
		{
			if(ctxt.state == State::AfterExpandStringPipe)
			{ throw std::runtime_error{"Junk after expand string pipe"}; }

			if(ctxt.state == State::AfterCommand) { throw std::runtime_error{"Junk after command"}; }

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
						ctxt.state = State::ExpandStringFirstCommand;
						break;

					case ',': cmd.add(Literal{std::move(ctxt.buffer)}); break;

					case ')':
						if(std::size(ctxt.buffer) != 0) { cmd.add(Literal{std::move(ctxt.buffer)}); }
						if(std::size(contexts) != 0)
						{
							auto& ctxt_prev = contexts.top();
							auto& cmd_prev = *std::get_if<Command>(&ctxt_prev.node.back());
							auto& expand_string = *std::get_if<ExpandString>(&cmd_prev.back());
							expand_string.command().pipe(std::move(ctxt.node));
							ctxt = std::move(contexts.top());
							ctxt.state = State::AfterExpandStringPipe;
							contexts.pop();
						}
						else
						{
							ctxt.state = State::AfterCommand;
						}
						break;

					default: ctxt.buffer += ch_in;
				}
				break;
			}

			case State::ExpandStringFirstCommand:
			{
				switch(ch_in)
				{
					case '(':
						contexts.push(std::move(ctxt));
						ctxt.state = State::ArgList;
						ctxt.node |= Command{std::move(contexts.top().buffer)};
						break;

					case '{':
						++ctxt.bracecount;
						ctxt.buffer += ch_in;
						break;

					case '}':
						if(std::size(ctxt.buffer) == 0)
						{
							ctxt.state = State::AfterExpandString;
							break;
						}
						else
						{
							if(ctxt.bracecount == 0) { throw std::runtime_error{"Curly-brace mismatch"}; }
							ctxt.buffer += ch_in;
							--ctxt.bracecount;
						}
						break;

					default: ctxt.buffer += ch_in;
				};
				break;
			}

			case State::AfterExpandStringPipe:
				switch(ch_in)
				{
					case '/': ctxt.state = State::Separator; break;

					case '}': ctxt.state = State::AfterExpandString; break;

					default: throw std::runtime_error{"Junk after expand string pipe"};
				}
				break;

			case State::Separator:
				switch(ch_in)
				{
					case '}':
					{
						if(std::size(ctxt.buffer) != 1)
						{ throw std::runtime_error{"Expected field separator of length 1"}; }
						auto& cmd = *std::get_if<Command>(&ctxt.node.back());
						auto& expand_string = *std::get_if<ExpandString>(&cmd.back());
						expand_string.command().separator(ctxt.buffer[0]);
						ctxt.buffer.clear();
						ctxt.state = State::AfterExpandString;
						break;
					}
					default: ctxt.buffer += ch_in;
				}

				break;

			case State::AfterExpandString:
			{
				auto& cmd = *std::get_if<Command>(&ctxt.node.back());
				auto& expand_string = *std::get_if<ExpandString>(&cmd.back());
				switch(ch_in)
				{
					case ')':
						expand_string.suffix(Literal{std::move(ctxt.buffer)});
						if(std::size(contexts) != 0)
						{
							auto& ctxt_prev = contexts.top();
							auto& cmd_prev = *std::get_if<Command>(&ctxt_prev.node.back());
							auto& expand_string = *std::get_if<ExpandString>(&cmd_prev.back());
							expand_string.command().pipe(std::move(ctxt.node));
							ctxt = std::move(contexts.top());
							ctxt.state = State::AfterExpandStringPipe;
							contexts.pop();
						}
						else
						{
							ctxt.state = State::AfterCommand;
						}
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

					case ';': return std::pair{ctxt.node, str};

					default: throw std::runtime_error{"Junk after command"};
				}
				break;


			case State::Escape:
				ctxt.buffer += ch_in;
				ctxt.state = ctxt.state_prev;
				break;
		}
	}
}