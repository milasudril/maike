//@	{
//@	 "targets":[{"name": "command.test", "type":"application", "autorun":1}]
//@	}

#include "./command.hpp"

#include <cassert>
#include <algorithm>

namespace
{
	struct Data
	{
		std::vector<std::string> variable;
	};

	struct MyInvoker
	{
		std::vector<std::byte> execp(Maike::fs::path const& pathname,
		                             std::vector<std::string> const& args,
		                             std::vector<std::byte> const&) const
		{
			printf("execp: %s", pathname.c_str());
			std::for_each(
			   std::begin(args), std::end(args), [](auto const& item) { printf(" %s", item.c_str()); });
			printf("\n");
			return std::vector<std::byte>{};
		}

		std::vector<std::string> const& getvar(std::string_view argname) const
		{
			printf("getvar: %s\n", argname.data());
			return data.get().variable;
		}

		std::reference_wrapper<Data> data;
	};
}

namespace Testcases
{
	void maikeCommandInterpreterParse()
	{
		namespace CI = Maike::CommandInterpreter;

		constexpr char const* test =
		   "cat({$source_file}) | g++(-, -x, c++, -std=c++17, foo{pkg-config(--libs, "
		   "gtk+-3)/~ }"
		   "bar, -o, foo.o, a{fo{o}(a, {test()/!}, {kaka()/%})/:}b, a{}b, {$dependencies}); "
		   "next_command({inner_command({$varname})})";
		auto res_1 = CI::makePipe(test);
		auto res_2 = CI::makePipe(res_1.second);

		auto const val_1 =
		   (CI::Pipe{}
		    | CI::Command{"cat"}.add(CI::ExpandString{CI::Literal{""}}.value(CI::Varname{"source_file"}))
		    | CI::Command{"g++"}
		         .add(CI::Literal{"-"})
		         .add(CI::Literal{"-x"})
		         .add(CI::Literal{"c++"})
		         .add(CI::Literal{"-std=c++17"})
		         .add(CI::ExpandString{CI::Literal{"foo"}}
		                 .suffix(CI::Literal{"bar"})
		                 .value(CI::CommandSplitOutput{}.separator(' ').pipe(
		                    CI::Pipe{}
		                    | CI::Command{"pkg-config"}
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
		         .add(CI::ExpandString{CI::Literal{"a"}}.suffix(CI::Literal{"b"}))
		         .add(CI::ExpandString{CI::Literal{""}}.value(CI::Varname{"dependencies"})));

		assert(res_1.first == val_1);
		assert(
		   (res_2.first
		    == (CI::Pipe{}
		        | CI::Command{"next_command"}.add(CI::ExpandString{CI::Literal{""}}.value(
		           CI::CommandSplitOutput{}.pipe(CI::Pipe{}
		                                         | CI::Command{"inner_command"}.add(CI::ExpandString{
		                                            CI::Literal{""}}.value(CI::Varname{"varname"}))))))));
		assert(*res_2.second == '\0');

		Data data;
		MyInvoker invoker{data};
		execute(res_1.first, CI::Invoker{std::ref(invoker)}, CI::CommandOutput{});
	}
}

int main()
{
	Testcases::maikeCommandInterpreterParse();
	return 0;
}