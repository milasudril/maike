//@	{
//@	 "targets":[{"name":"reader.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./reader.hpp"

#include <cassert>
#include <string_view>
#include <array>
#include <cstring>

namespace
{
	struct Source
	{
		std::string_view data;
		size_t read_offset;
	};

	size_t read(Source& src, std::byte* buff, size_t N)
	{
		auto n = std::min(N, src.data.size() - src.read_offset);
		memcpy(buff, src.data.data() + src.read_offset, n);
		src.read_offset += n;
		return n;
	}
}

namespace Testcases
{
	void maikeReaderReadStuff()
	{
		Source src{"Hello, World", 0};
		Maike::Io::Reader reader{src};

		std::array<std::byte, 12> dest;
		auto n = read(reader, std::data(dest), 5);
		assert(n == 5);
		n = read(reader, std::data(dest) + n, 24);
		assert(n == 7);
		assert(src.read_offset == src.data.size());
		assert(memcmp("Hello, World", std::data(dest), 12) == 0);
	}
}

int main()
{
	Testcases::maikeReaderReadStuff();
	return 0;
}