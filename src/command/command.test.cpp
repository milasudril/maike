//@	{
//@	 "targets":[{"name": "command.test", "type":"application", "autorun":1}]
//@	}

#include "./command.hpp"

#include <cassert>

namespace Testcases
{
	void maikeCommandInterpreterParse()
	{
		namespace CI = Maike::CommandInterpreter;

		constexpr char const* test =
		   "system.cat(foo.cpp) | system.g++(-, -x, c++, -std=c++17, foo{system.pkg-config(--libs, "
		   "gtk+-3)/~ }"
		   "bar, -o, foo.o, a{fo{o}(a, {test()/!}, {kaka()/%})/:}b, a{}b); next_command()";
		auto res_1 = CI::makePipe(test);
		auto res_2 = CI::makePipe(res_1.second);

		auto const val_1 =
		   (CI::Pipe{} | CI::Command{"system.cat"}.add(CI::Literal{"foo.cpp"})
		    | CI::Command{"system.g++"}
		         .add(CI::Literal{"-"})
		         .add(CI::Literal{"-x"})
		         .add(CI::Literal{"c++"})
		         .add(CI::Literal{"-std=c++17"})
		         .add(CI::ExpandString{CI::Literal{"foo"}}
		                 .suffix(CI::Literal{"bar"})
		                 .value(CI::CommandSplitOutput{}.separator(' ').pipe(
		                    CI::Pipe{}
		                    | CI::Command{"system.pkg-config"}
		                         .add(CI::Literal{"--libs"})
		                         .add(CI::Literal{"gtk+-3"}))))
		         .add(CI::Literal{"-o"})
		         .add(CI::Literal{"foo.o"})
		         .add(CI::ExpandString{CI::Literal{"a"}}
		                 .suffix(CI::Literal{"b"})
		                 .value(CI::CommandSplitOutput{}.separator(':').pipe(
		                    CI::Pipe{}
		                    | CI::Command{"fo{o}"}
		                         .add(CI::Literal{"a"})
		                         .add(CI::ExpandString{CI::Literal{""}}
		                                 .suffix(CI::Literal{""})
		                                 .value(CI::CommandSplitOutput{}.separator('!').pipe(
		                                    CI::Pipe{} | CI::Command{"test"})))
		                         .add(CI::ExpandString{CI::Literal{""}}
		                                 .suffix(CI::Literal{""})
		                                 .value(CI::CommandSplitOutput{}.separator('%').pipe(
		                                    CI::Pipe{} | CI::Command{"kaka"}))))))
		         .add(CI::ExpandString{CI::Literal{"a"}}.suffix(CI::Literal{"b"})));

		assert(res_1.first == val_1);
		assert((res_2.first == (CI::Pipe{} | CI::Command{"next_command"})));
		assert(*res_2.second == '\0');
	}
}

int main()
{
	Testcases::maikeCommandInterpreterParse();
	return 0;
}