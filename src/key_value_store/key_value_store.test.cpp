//@	{
//@	  "targets":[{"name":"key_value_store.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./key_value_store.hpp"

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
	void maikeKeyValueStoreObjectCreateEmptySource()
	{
		Maike::KeyValueStore::Object test{EmptySource{}};
		assert(test.empty());
	}

	void maikeKeyValueStoreObjectCreateInvalidJson()
	{
		try
		{
			Maike::KeyValueStore::Object test{StringViewSource{R"json(
	{"invalid json": "missing curly brace")json"}};
			abort();
		}
		catch(...)
		{}
	}

	void maikeKeyValueStoreObjectCreateFromJson()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json({
"an object": {"key": "some value"},
"an array": ["this", "is", "an", "array"],
"a string": "This is a string",
"an integer": 123,
"a double": 3.14
})json"}};

		assert(!test.empty());
		auto ref = test.get().as<Maike::KeyValueStore::CompoundRefConst>();

		{
			auto obj = ref.get<Maike::KeyValueStore::CompoundRefConst>("an object");
			assert(std::string_view{"some value"} == obj.get<char const*>("key"));
		}

		{
			auto array = ref.get<Maike::KeyValueStore::ArrayRefConst>("an array");
			std::vector<std::string_view> vals{"this", "is", "an", "array"};
			assert((std::equal(std::begin(array), std::end(array), std::begin(vals), [](auto a, auto b) {
				return b == a.template as<char const*>();
			})));
		}

		{
			auto string = ref.get<char const*>("a string");
			assert(std::string_view{"This is a string"} == string);
		}

		{
			auto integer = ref.get<json_int_t>("an integer");
			assert(123 == integer);
		}

		{
			auto a_double = ref.get<double>("a double");
			assert(3.14 == a_double);
		}
	}
}

int main()
{
	Testcases::maikeKeyValueStoreObjectCreateEmptySource();
	Testcases::maikeKeyValueStoreObjectCreateInvalidJson();
	Testcases::maikeKeyValueStoreObjectCreateFromJson();
	return 0;
}