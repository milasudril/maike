//@	{
//@	 "targets":[{"name":"execve.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./execve.hpp"

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

		size_t operator()(std::byte* buffer, size_t n, Maike::Io::Redirector::StdIn)
		{
			auto const n_copy = std::min(n, m_stdin_bytes_left);
			if(n_copy == 0) { return n_copy; }

			memcpy(buffer, m_stdin_read_ptr, n_copy);
			m_stdin_read_ptr += n_copy;
			m_stdin_bytes_left -= n_copy;
			return n_copy;
		}

		void operator()(std::byte const* buffer, size_t n, Maike::Io::Redirector::StdOut)
		{
			while(n)
			{
				m_stdout += static_cast<char>(*buffer);
				++buffer;
				--n;
			}
		}

		void operator()(std::byte const* buffer, size_t n, Maike::Io::Redirector::StdErr)
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
	void maikeLocalExecveTestArgs()
	{
		IoRedirectorTest redir{"The text here does not matter"};

		auto status = Maike::Exec::execve("/bin/sh", {"-c", "echo Hello, World"}, redir);
		assert(status.returnedFromMain());
		assert(status.returnCode() == 0);
		assert(redir.stdout() == "Hello, World\n");
	}

	void maikeLocalExecveNonExistingExe()
	{
		try
		{
			(void)Maike::Exec::execve("This file does not exsit", {}, IoRedirectorTest{""});
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeLocalExecveTestPipesAndExitStatus()
	{
		IoRedirectorTest redir{R"bash(echo "This is some text written to stdout"
echo "This is some text written to stderr" 1>&2
exit 123
)bash"};

		auto status = Maike::Exec::execve("/bin/sh", {}, redir);
		assert(status.returnedFromMain());
		assert(status.returnCode() == 123);

		assert(redir.stderr() == "This is some text written to stderr\n");
		assert(redir.stdout() == "This is some text written to stdout\n");
	}

	void maikeLocalExecveTestPipesAndExitStatusMt5Times()
	{
		for(int k = 0; k < 5; ++k)
		{
			std::vector<std::thread> threads;
			for(int k = 0; k < 16; ++k)
			{
				threads.emplace_back(maikeLocalExecveTestPipesAndExitStatus);
			}
			std::for_each(std::begin(threads), std::end(threads), [](auto& x) { x.join(); });
			putc('*', stderr);
		}
		putc('\n', stderr);
	}

	void maikeLocalExecveSimpleApiNoStdin()
	{
		auto res = Maike::Exec::execve("/bin/sh", {"-c", "echo 'Hej'; echo 'Då' 1>&2"});
		std::string stdout;
		std::string stderr;
		std::transform(res.stdout().begin(),
		               res.stdout().end(),
		               std::back_inserter(stdout),
		               [](auto val) { return static_cast<char>(val); });
		std::transform(res.stderr().begin(),
		               res.stderr().end(),
		               std::back_inserter(stderr),
		               [](auto val) { return static_cast<char>(val); });
		assert(stdout == "Hej\n");
		assert(stderr == "Då\n");
		assert(res.exitStatus().returnCode() == 0);
	}
}

int main()
{
	Testcases::maikeLocalExecveTestArgs();
	Testcases::maikeLocalExecveNonExistingExe();
	Testcases::maikeLocalExecveTestPipesAndExitStatus();
	Testcases::maikeLocalExecveSimpleApiNoStdin();
	Testcases::maikeLocalExecveTestPipesAndExitStatusMt5Times();
}