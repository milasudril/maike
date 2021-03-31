//@	{
//@	 "targets":[{"name": "command.o", "type":"object"}]
//@	}

#include "./command.hpp"

Maike::CommandInterpreter::Command Maike::CommandInterpreter::makeCommand(char const* str)
{
	Command cmd;
	while(true)
	{
		auto ch_in = *str;
		if(ch_in == '\0')
		{
			return cmd;
		}
	}
}