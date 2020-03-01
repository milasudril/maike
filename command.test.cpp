//@	{
//@	 "targets":[{"name":"command.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./command.hpp"

#include <cassert>
#include <cstring>
#include <thread>
#include <algorithm>

namespace
{
	class IoRedirectorTest
	{
	public:
		explicit IoRedirectorTest(std::string&& stdin): m_stdin{std::move(stdin)}
		{
			m_stdin_read_ptr = m_stdin.data();
			m_stdin_bytes_left = m_stdin.size();
		}

		size_t operator()(std::byte* buffer, size_t n, Maike::Command::StdIn)
		{
			auto const n_copy = std::min(n, m_stdin_bytes_left);
			if(n_copy == 0) { return n_copy; }

			memcpy(buffer, m_stdin_read_ptr, n_copy);
			m_stdin_read_ptr += n_copy;
			m_stdin_bytes_left -= n_copy;
			return n_copy;
		}

		void operator()(std::byte const* buffer, size_t n, Maike::Command::StdOut)
		{
			while(n)
			{
				m_stdout += static_cast<char>(*buffer);
				++buffer;
				--n;
			}
		}

		void operator()(std::byte const* buffer, size_t n, Maike::Command::StdErr)
		{
			while(n)
			{
				m_stderr += static_cast<char>(*buffer);
				++buffer;
				--n;
			}
		}

		std::string const& stderr() const
		{
			return m_stderr;
		}

		std::string const& stdout() const
		{
			return m_stdout;
		}


	private:
		std::string m_stdin;
		char const* m_stdin_read_ptr;
		size_t m_stdin_bytes_left;

		std::string m_stdout;

		std::string m_stderr;
	};
}

namespace Testcases
{
	void maikeCommandCreate()
	{
		std::vector<std::string> const command_args{"foo", "arg1"};
		Maike::fs::path const exe{"bash"};

		Maike::Command cmd{exe, command_args};

		assert(cmd.executable() == exe);
		assert(cmd.args() == command_args);

		IoRedirectorTest redir{"Hello, World"};

		auto status = cmd.execp(redir);
		if(status.returnedFromMain())
		{
			if(status.exitStatus() > 0) { assert(redir.stderr().size() > 0); }
		}
	}

	void maikeCommandNonExistingExe()
	{
		try
		{
			Maike::Command cmd{"This file does not exsit", {}};
			(void)cmd.execp(IoRedirectorTest{""});
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeCommandTestPipesAndExitStatus()
	{
		Maike::fs::path const exe{"bash"};

		Maike::Command cmd{"bash", {}};

		IoRedirectorTest redir{R"bash(echo "This is some text written to stdout"
echo "This is some text written to stderr" 1>&2
exit 123
)bash"};

		auto status = cmd.execp(redir);
		assert(status.returnedFromMain());
		assert(status.exitStatus() == 123);

		assert(redir.stderr() == "This is some text written to stderr\n");
		assert(redir.stdout() == "This is some text written to stdout\n");
	}

	void maikeCommandTestPipesAndExitStatusMt5Times()
	{
		for(int k = 0; k < 5; ++k)
		{
			std::vector<std::thread> threads;
			for(int k = 0; k < 16; ++k)
			{
				threads.emplace_back(maikeCommandTestPipesAndExitStatus);
			}
			std::for_each(std::begin(threads), std::end(threads), [](auto& x) { x.join(); });
			putc('*', stderr);
		}
		putc('\n', stderr);
	}
}

int main()
{
	Testcases::maikeCommandCreate();
	Testcases::maikeCommandNonExistingExe();
	Testcases::maikeCommandTestPipesAndExitStatus();
	Testcases::maikeCommandTestPipesAndExitStatusMt5Times();
}