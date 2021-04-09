//@	{
//@	 "targets":[{"name": "command.test", "type":"application", "autorun":1}]
//@	}

#include "./command.hpp"

#include <cassert>

namespace Testcases
{
	void maikeCommandInterpreterParseEmptyString()
	{
		constexpr char const* test =
		   "system.cat(foo.cpp)|system.g++(-, -x, c++, -std=c++17, foo{system.pkg-config(--libs, "
		   "gtk+-3)/~ "
		   "}bar, -o, foo.o)";
		auto pipe = Maike::CommandInterpreter::makePipe(test);

		printf("%s\n", std::get_if<Maike::CommandInterpreter::Command>(&pipe.back())->name().c_str());
		//		assert(cmd == Maike::CommandInterpreter::Command{});
	}
}

int main()
{
	Testcases::maikeCommandInterpreterParseEmptyString();
	return 0;
}