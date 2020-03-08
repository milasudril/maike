//@	{"targets":[{"name":"command.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./command.hpp"

#include <cassert>

namespace
{
	struct TestInvoker
	{
	};
	struct TestIoRedirector
	{
		Maike::fs::path exe;
		std::vector<std::string> args;
	};

	int execp(TestInvoker const&,
	          Maike::fs::path const& exe,
	          std::vector<std::string> const& args,
	          TestIoRedirector& io_redir)
	{
		io_redir.exe = exe;
		io_redir.args = args;
		return 123;
	}
}

namespace Testcases
{
	void maikeCommandInitVals()
	{
		Maike::Command cmd{"foobar", {"bulle", "kaka"}};
		assert(cmd.executable() == "foobar");
		assert((cmd.args() == std::vector<std::string>{"bulle", "kaka"}));
	}

	void maikeCommandExecp()
	{
		Maike::Command cmd{"foobar", {"bulle", "kaka"}};
		TestInvoker invoker;
		TestIoRedirector redir;
		auto res = cmd.execp(invoker, redir);
		assert(res == 123);
		assert(redir.exe == cmd.executable());
		assert(redir.args == cmd.args());
	}
}

int main()
{
	Testcases::maikeCommandInitVals();
	Testcases::maikeCommandExecp();
	return 0;
}
