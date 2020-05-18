//@	{
//@	 "targets":[{"name":"output_buffer.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "output_buffer.hpp"

#include <cassert>
#include <vector>
#include <functional>
#include <cstring>

namespace
{
	struct Sink
	{
		std::vector<std::byte> data;
	};

	void write(Sink& sink, std::byte const* buffer, size_t n)
	{
		sink.data.insert(std::end(sink.data), buffer, buffer + n);
	}
}

namespace Testcases
{
	void maikeOutputBufferWrite()
	{
		Sink test;
		Maike::Io::OutputBuffer<Sink, 4> buffer{std::ref(test)};
		char const* input = "Hello, World";
		while(*input != 0)
		{
			buffer.putchar(static_cast<std::byte>(*input));
			++input;
		}
		assert(test.data.size() == strlen(input));
		test.data.push_back(static_cast<std::byte>(0));
		assert(strcmp("Hello, World", reinterpret_cast<char const*>(test.data.data())));
	}

	void maikeOutputBufferWriteLargeBuffer()
	{
		Sink test;
		Maike::Io::OutputBuffer<Sink, 4096> buffer{std::ref(test)};
		char const* input = "Hello, World";
		while(*input != 0)
		{
			buffer.putchar(static_cast<std::byte>(*input));
			++input;
		}
		assert(test.data.size() == strlen(input));
		test.data.push_back(static_cast<std::byte>(0));
		assert(strcmp("Hello, World", reinterpret_cast<char const*>(test.data.data())));
	}
}

int main()
{
	Testcases::maikeOutputBufferWrite();
	Testcases::maikeOutputBufferWriteLargeBuffer();
}