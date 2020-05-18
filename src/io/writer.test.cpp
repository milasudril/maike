//@	{
//@	 "targets":[{"name":"writer.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./writer.hpp"

#include <cassert>
#include <string_view>
#include <array>
#include <cstring>

static_assert(Maike::Io::TaggedWriter<12>::Tag == 12);

namespace
{
	struct Sink
	{
		std::string buffer;
	};

	void write(Sink& dest, std::byte const* buff, size_t n)
	{
		dest.buffer.append(reinterpret_cast<char const*>(buff), n);
	}
}

namespace Testcases
{
	void maikeWriterReadStuff()
	{
		Sink dest;
		Maike::Io::Writer writer{dest};
		write(dest, reinterpret_cast<std::byte const*>("Hello"), 5);
		assert(dest.buffer == "Hello");
		write(dest, reinterpret_cast<std::byte const*>(", World"), 7);
		assert(dest.buffer == "Hello, World");
	}
}

int main()
{
	Testcases::maikeWriterReadStuff();
	return 0;
}