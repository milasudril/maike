//@	{
//@	  "targets":[{"name":"tag_filter.test.cpp","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./tag_filter.hpp"

#include <cassert>
#include <cstring>
#include <array>
#include <vector>

namespace
{
	struct DummyReader
	{};

	size_t read(DummyReader, std::byte* buffer, size_t n)
	{
		assert(n == 11);
		memcpy(buffer, "Hello World", 11);
		return 5;
	}

	struct DummyWriter
	{
		std::vector<std::byte> output;
	};

	size_t write(DummyWriter& w, std::byte const* buffer, size_t n)
	{
		w.output.insert(std::end(w.output), buffer, buffer + n);
		return n;
	}

	struct TestFilter
	{
	};

	void run(TestFilter const&, Maike::Reader reader, Maike::SourceOutStream src, Maike::TagOutStream tags)
	{
		std::array<std::byte, 11> buffer;
		read(reader, buffer.data(), buffer.size());
		write(src, buffer.data(), 5);
		write(tags, buffer.data() + 6, 5);
	}
}

namespace Testcases
{
	void maikeTagFilterCreateAndRun()
	{
		TestFilter f;
		Maike::TagFilter filter{f};

		DummyReader r;
		DummyWriter source;
		DummyWriter tags;

 		filter.run(Maike::Reader{r}, Maike::SourceOutStream{source}, Maike::TagOutStream{tags});

 		std::vector<std::byte> const source_expected{static_cast<std::byte>('H'),
											static_cast<std::byte>('e'),
											static_cast<std::byte>('l'),
											static_cast<std::byte>('l'),
											static_cast<std::byte>('o')};

		std::vector<std::byte> const tags_expected{static_cast<std::byte>('W'),
									static_cast<std::byte>('o'),
									static_cast<std::byte>('r'),
									static_cast<std::byte>('l'),
									static_cast<std::byte>('d')};

		assert(source.output == source_expected);
		assert(tags.output == tags_expected);
	}
}

int main()
{
	void maikeTagFilterCreateAndRun();
	return 0;
}