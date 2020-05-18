//@	{"targets":[{"name":"mem_io_redirector.test", "type":"application", "autorun":1}]}

#undef NDEBUG

#include "mem_io_redirector.hpp"

#include <array>
#include <cassert>

namespace Testcases
{
	void maikeIoMemRedirectorStdin()
	{
		std::array<std::byte, 16> input{std::byte(0x0),
		                                std::byte(0x1),
		                                std::byte(0x2),
		                                std::byte(0x3),
		                                std::byte(0x4),
		                                std::byte(0x5),
		                                std::byte(0x6),
		                                std::byte(0x7),
		                                std::byte(0x8),
		                                std::byte(0x9),
		                                std::byte(0xa),
		                                std::byte(0xb),
		                                std::byte(0xc),
		                                std::byte(0xd),
		                                std::byte(0xe),
		                                std::byte(0xf)};
		Maike::Io::MemRedirector redir{input.data(), input.size()};

		std::array<std::byte, 16> output{};
		auto n = redir(output.data(), 10, Maike::Io::Redirector::StdIn{});
		assert(n == 10);
		assert(memcmp(input.data(), output.data(), 10) == 0);

		n = redir(output.data() + n, 10, Maike::Io::Redirector::StdIn{});
		assert(n == 6);
		assert(memcmp(output.data(), input.data(), 16) == 0);
	}

	void maikeIoMemRedirectorStdoutStderr()
	{
		std::array<std::byte, 16> stdout{std::byte(0x0),
		                                 std::byte(0x1),
		                                 std::byte(0x2),
		                                 std::byte(0x3),
		                                 std::byte(0x4),
		                                 std::byte(0x5),
		                                 std::byte(0x6),
		                                 std::byte(0x7),
		                                 std::byte(0x8),
		                                 std::byte(0x9),
		                                 std::byte(0xa),
		                                 std::byte(0xb),
		                                 std::byte(0xc),
		                                 std::byte(0xd),
		                                 std::byte(0xe),
		                                 std::byte(0xf)};

		std::array<std::byte, 13> stderr{std::byte(0x10),
		                                 std::byte(0x11),
		                                 std::byte(0x12),
		                                 std::byte(0x13),
		                                 std::byte(0x14),
		                                 std::byte(0x15),
		                                 std::byte(0x16),
		                                 std::byte(0x17),
		                                 std::byte(0x18),
		                                 std::byte(0x19),
		                                 std::byte(0x1a),
		                                 std::byte(0x1b),
		                                 std::byte(0x1c)};

		Maike::Io::MemRedirector redir{nullptr, 123};

		redir(stdout.data(), 10, Maike::Io::Redirector::StdOut{});
		assert(redir.stdout().size() == 10);
		assert(memcmp(redir.stdout().data(), stdout.data(), 10) == 0);

		redir(stderr.data(), 5, Maike::Io::Redirector::StdErr{});
		assert(redir.stderr().size() == 5);
		assert(memcmp(redir.stderr().data(), stderr.data(), 5) == 0);

		redir(stdout.data() + 10, 6, Maike::Io::Redirector::StdOut{});
		redir(stderr.data() + 5, 8, Maike::Io::Redirector::StdErr{});

		assert(redir.stdout().size() == 16);
		assert(memcmp(redir.stdout().data(), stdout.data(), 16) == 0);

		assert(redir.stderr().size() == 13);
		assert(memcmp(redir.stderr().data(), stderr.data(), 13) == 0);
	}
}

int main()
{
	Testcases::maikeIoMemRedirectorStdin();
	Testcases::maikeIoMemRedirectorStdoutStderr();
	return 0;
}
