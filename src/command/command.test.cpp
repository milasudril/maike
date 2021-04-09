//@	{
//@	 "targets":[{"name": "command.test", "type":"application", "autorun":1}]
//@	}

#include "./command.hpp"

#include <cassert>

namespace Testcases
{
	void maikeCommandInterpreterParseOneCommand()
	{
		constexpr char const* test =
		   "system.cat(foo.cpp)|system.g++(-, -x, c++, -std=c++17, foo{system.pkg-config(--libs, "
		   "gtk+-3)/~ }"
		   "bar, -o, foo.o, a{fo{o}(a, {test()/!}, {kaka()/%})/:}b, a{}b)";
		auto pipe = Maike::CommandInterpreter::makePipe(test);

		printf("%s\n", std::get_if<Maike::CommandInterpreter::Command>(&pipe.back())->name().c_str());
		//		assert(cmd == Maike::CommandInterpreter::Command{});
	}
}

int main()
{
	Testcases::maikeCommandInterpreterParseOneCommand();
	return 0;
}