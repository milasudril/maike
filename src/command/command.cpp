//@	{
//@	 "targets":[{"name": "command.o", "type":"object"}]
//@	}

#include "./command.hpp"

#include <string>

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