//@	{"targets":[{"name":"command.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./command.hpp"

#include <cassert>
#include <cstring>

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

	struct TestSource
	{
		explicit TestSource(char const* buffer):begin{buffer},read_ptr{buffer}{}
		char const* const begin;
		char const* read_ptr;
	};

	size_t read(TestSource& s, std::byte* buffer, size_t N)
	{
		auto n = std::min(N, static_cast<size_t>(s.read_ptr - s.begin));
		memcpy(buffer,s.read_ptr, n);
		s.read_ptr += n;
		return n;
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

	void maikeCommandFromJson()
	{
		TestSource src{R"_({
    "args": [
        "bulle",
        "kaka"
    ],
    "executable": "foobar"
})_"};

		auto json = Maike::KeyValueStore::jsonLoad(src, "");
		auto cmd = json.as<Maike::Command>();
		assert(cmd.executable() == "foobar");
		assert((cmd.args() == std::vector<std::string>{"bulle", "kaka"}));
	}
}

int main()
{
	Testcases::maikeCommandInitVals();
	Testcases::maikeCommandExecp();
	Testcases::maikeCommandToJson();
	return 0;
}
