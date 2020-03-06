//@	{
//@	 "targets":[{"name":"local_execp.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./local_execp.hpp"

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

		size_t operator()(std::byte* buffer, size_t n, Maike::IoRedirector::StdIn)
		{
			auto const n_copy = std::min(n, m_stdin_bytes_left);
			if(n_copy == 0) { return n_copy; }

			memcpy(buffer, m_stdin_read_ptr, n_copy);
			m_stdin_read_ptr += n_copy;
			m_stdin_bytes_left -= n_copy;
			return n_copy;
		}

		void operator()(std::byte const* buffer, size_t n, Maike::IoRedirector::StdOut)
		{
			while(n)
			{
				m_stdout += static_cast<char>(*buffer);
				++buffer;
				--n;
			}
		}

		void operator()(std::byte const* buffer, size_t n, Maike::IoRedirector::StdErr)
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
	void maikeLocalExcepTestArgs()
	{
		IoRedirectorTest redir{"The text here does not matter"};

		auto status = Maike::execp("bash", {"-c", "echo Hello, World"}, redir);
		assert(status.returnedFromMain());
		assert(status.exitStatus() == 0);
		assert(redir.stdout() == "Hello, World\n");
	}

	void maikeLocalExcepNonExistingExe()
	{
		try
		{
			(void)Maike::execp("This file does not exsit", {}, IoRedirectorTest{""});
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeLocalExcepTestPipesAndExitStatus()
	{
		IoRedirectorTest redir{R"bash(echo "This is some text written to stdout"
echo "This is some text written to stderr" 1>&2
exit 123
)bash"};

		auto status = Maike::execp("bash", {}, redir);
		assert(status.returnedFromMain());
		assert(status.exitStatus() == 123);

		assert(redir.stderr() == "This is some text written to stderr\n");
		assert(redir.stdout() == "This is some text written to stdout\n");
	}

	void maikeLocalExcepTestPipesAndExitStatusMt5Times()
	{
		for(int k = 0; k < 5; ++k)
		{
			std::vector<std::thread> threads;
			for(int k = 0; k < 16; ++k)
			{
				threads.emplace_back(maikeLocalExcepTestPipesAndExitStatus);
			}
			std::for_each(std::begin(threads), std::end(threads), [](auto& x) { x.join(); });
			putc('*', stderr);
		}
		putc('\n', stderr);
	}
}

int main()
{
	Testcases::maikeLocalExcepTestArgs();
	Testcases::maikeLocalExcepNonExistingExe();
	Testcases::maikeLocalExcepTestPipesAndExitStatus();
	Testcases::maikeLocalExcepTestPipesAndExitStatusMt5Times();
}