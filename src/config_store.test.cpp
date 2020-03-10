//@	{
//@	  "targets":[{"name":"config_store.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./config_store.hpp"

#include <cassert>
#include <string_view>
#include <cstring>
#include <vector>
#include <algorithm>

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
"an object": {"key": "some value"},
"an array": ["this", "is", "an", "array"],
"a string": "This is a string",
"an integer": 123,
"a double": 3.14,
"a bool": true,
"a null value": null
})json"}};

		assert(!test.empty());
		auto ref = test.get().as<Maike::ConfigObjectCompoundConstRef>();

		{
			auto obj = ref.get<Maike::ConfigObjectCompoundConstRef>("an object");
			assert(std::string_view{"some value"} == obj.get<char const*>("key"));
		}

		{
			auto array = ref.get<Maike::ConfigObjectArrayRefConst>("array");
			std::vector<std::string_view> vals{"this", "is", "an", "array"};
			assert((std::equal(std::begin(array), std::end(array), std::begin(vals), [](auto a, auto b) {
				return b == a.template as<char const*>();
			})));
		}
	}
}

int main()
{
	Testcases::maikeConfigStoreCreateEmptySource();
	Testcases::maikeConfigStoreCreateInvalidJson();
	Testcases::maikeConfigStoreCreateFromJson();
	return 0;
}