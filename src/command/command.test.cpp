//@	{
//@	 "targets":[{"name": "command.test", "type":"application", "autorun":1}]
//@	}

#include "./command.hpp"

#include <cassert>

namespace Testcases
{
	void maikeCommandInterpreterParseEmptyString()
	{
		auto cmd = Maike::CommandInterpreter::makeCommand("");
		assert(cmd == Maike::CommandInterpreter::Command{});
	}
}

int main()
{
	Testcases::maikeCommandInterpreterParseEmptyString();
	return 0;
}