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

	struct TestSink
	{
		std::string buffer;
	};

	void write(TestSink& s, char const* buffer, size_t buff_len)
	{
		s.buffer.append(buffer, buff_len);
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
		auto res = cmd.invoke(invoker, redir);
		assert(res == 123);
		assert(redir.exe == cmd.executable());
		assert(redir.args == cmd.args());
	}

	void maikeCommandToJson()
	{
		Maike::Command cmd{"foobar", {"bulle", "kaka"}};
		assert(cmd.executable() == "foobar");
		assert((cmd.args() == std::vector<std::string>{"bulle", "kaka"}));

		auto json = toJson(cmd);
		TestSink sink;
		store(json, sink);

		char const* res = R"_({
    "args": [
        "bulle",
        "kaka"
    ],
    "executable": "foobar"
})_";
		assert(sink.buffer == res);
	}
}

int main()
{
	Testcases::maikeCommandInitVals();
	Testcases::maikeCommandExecp();
	Testcases::maikeCommandToJson();
	return 0;
}
