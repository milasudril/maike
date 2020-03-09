//@	{
//@	  "targets":[{"name":"config_store.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./config_store.hpp"

#include <cassert>
#include <string_view>
#include <cstring>

namespace
{
	struct EmptySource{};

	size_t read(EmptySource, std::byte*, size_t)
	{return 0;}

	char const* name(EmptySource)
	{return "<input buffer>";}

	struct StringViewSource
	{
		explicit StringViewSource(std::string_view v):read_ptr{v.data()}, n_bytes_left{v.size()}{}
		char const* read_ptr;
		size_t n_bytes_left;
	};

	size_t read(StringViewSource& src, std::byte* buffer, size_t N)
	{
		auto const n = std::min(N, src.n_bytes_left);
		memcpy(buffer, src.read_ptr, n);
		src.read_ptr += n;
		src.n_bytes_left -= n;
		return n;
	}

	char const* name(StringViewSource)
	{
		return "<input buffer>";
	}
}

namespace Testcases
{
	void maikeConfigStoreCreateEmptySource()
	{
		Maike::ConfigStore test{EmptySource{}};
		assert(test.empty());
	}

	void maikeConfigStoreCreateInvalidJson()
	{
		try
		{
			Maike::ConfigStore test{StringViewSource{R"json(
	{"invalid json": "missing curly brace")json"}};
			abort();
		}
		catch(...)
		{}
	}

	void maikeConfigStoreCreateFromJson()
	{
		Maike::ConfigStore test{StringViewSource{R"json({
"a config store": {"key": "some value"},
"an array": ["this", "is", "an", "array"],
"a string": "This is a string",
"an integer": 123,
"a double": 3.14,
"a bool": true,
"a null value": null
})json"}};

		assert(!test.empty());
	}
}

int main()
{
	Testcases::maikeConfigStoreCreateEmptySource();
	Testcases::maikeConfigStoreCreateInvalidJson();
	Testcases::maikeConfigStoreCreateFromJson();
	return 0;
}