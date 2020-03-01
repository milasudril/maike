//@	{
//@	 "targets":[{"name":"command.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./command.hpp"

#include <cassert>
#include <cstring>

namespace
{
	class IoRedirectorTest
	{
	public:
		explicit IoRedirectorTest(std::string&& stdin): m_stdin{std::move(stdin)}
		{
			m_stdin_ptr = m_stdin.data();
		}

		size_t operator()(std::byte* buffer, size_t n, Maike::Command::StdIn)
		{
			auto n_copy = std::min(n, static_cast<size_t>(m_stdin_ptr - m_stdin.data()));
			memcpy(buffer, m_stdin_ptr, n_copy);
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
			return m_stderr;
		}


	private:
		std::string m_stdin;
		char const* m_stdin_ptr;

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

		auto status =cmd.execp(redir);
		if(status.returnedFromMain())
		{
			if(status.exitStatus() > 0)
			{ assert(redir.stderr().size() > 0); }
		}
	}
}

int main()
{
	Testcases::maikeCommandCreate();
}