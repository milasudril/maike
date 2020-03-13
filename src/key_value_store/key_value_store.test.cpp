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
#include <array>

#include <unistd.h>

namespace
{
	struct EmptySource
	{
	};

	size_t read(EmptySource, std::byte*, size_t)
	{
		return 0;
	}

	char const* name(EmptySource)
	{
		return "<input buffer>";
	}

	struct StringViewSource
	{
		explicit StringViewSource(std::string_view v): read_ptr{v.data()}, n_bytes_left{v.size()}
		{
		}
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

	struct TestType
	{
		std::string item;
		long long int count;
	};

	TestType get(Maike::KeyValueStore::Tag<TestType>, Maike::KeyValueStore::ObjectRefConst obj)
	{
		auto compound = obj.as<Maike::KeyValueStore::CompoundRefConst>();
		return TestType{compound.get<char const*>("item"), compound.get<json_int_t>("count")};
	}

	struct StringSink
	{
		std::string buffer;
	};

	void write(StringSink& sink, char const* data, size_t N)
	{
		sink.buffer.insert(std::end(sink.buffer), data, data + N);
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
		{
		}
	}

	void maikeKeyValueStoreObjectCreateDataAfterJson()
	{
		StringViewSource src{R"json(
{"Key": "value"}
Here is some junk)json"};
		Maike::KeyValueStore::Object test{src};
		assert(*src.read_ptr == ' ');
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
		auto ref = std::as_const(test).get().as<Maike::KeyValueStore::CompoundRefConst>();

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

	void maikeKeyValueStoreObjectGetHelperBuiltin()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json([1, 2, 3])json"}};
		std::array<int, 3> vals{1, 2, 3};
		auto array =
		   Maike::KeyValueStore::get<Maike::KeyValueStore::ArrayRefConst>(std::as_const(test).get());
		assert((std::equal(std::begin(array), std::end(array), std::begin(vals), [](auto a, auto b) {
			return Maike::KeyValueStore::get<json_int_t>(a) == b;
		})));
	}


	void maikeKeyValueStoreObjectGetHelperAdl()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json({
"item": "Foobar",
"count": 123
})json"}};

		auto item = std::as_const(test).get();

		TestType obj = get(Maike::KeyValueStore::Tag<TestType>{}, item);
		assert(obj.item == "Foobar");
		assert(obj.count == 123);
	}

	void maikeKeyValueStoreObjectGetHelperAdlInner()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json({
"subobject":{
	"item": "Foobar",
	"count": 123
}})json"}};

		auto item = std::as_const(test).get();

		TestType obj = item.as<Maike::KeyValueStore::CompoundRefConst>().get<TestType>("subobject");
		assert(obj.item == "Foobar");
		assert(obj.count == 123);
	}

	void maikeKeyValueStoreObjectNonExistingKey()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json({
"item": "Foobar",
"count": 123
})json"}};

		auto item = std::as_const(test).get();
		try
		{
			(void)item.as<Maike::KeyValueStore::CompoundRefConst>().get<char const*>("non-existing key");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeKeyValueStoreObjectWrongType()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json({
"an object": {"key": "some value"},
"an array": ["this", "is", "an", "array"],
"a string": "This is a string",
"an integer": 123,
"a double": 3.14
})json"}};

		auto ref = std::as_const(test).get().as<Maike::KeyValueStore::CompoundRefConst>();

		try
		{
			(void)ref.get<Maike::KeyValueStore::ArrayRefConst>("an object");
			abort();
		}
		catch(...)
		{
		}
		try
		{
			(void)ref.get<char const*>("an array");
			abort();
		}
		catch(...)
		{
		}
		try
		{
			(void)ref.get<json_int_t>("a string");
			abort();
		}
		catch(...)
		{
		}
		try
		{
			(void)ref.get<double>("an integer");
			abort();
		}
		catch(...)
		{
		}
		try
		{
			(void)ref.get<Maike::KeyValueStore::CompoundRefConst>("a double");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeKeyValueStoreObjectWrite()
	{
		Maike::KeyValueStore::Object test{StringViewSource{R"json({
"an object": {"key": "some value"},
"an array": ["this", "is", "an", "array"],
"a string": "This is a string",
"an integer": 123,
"a double": 3.14
})json"}};

		StringSink sink;
		write(test, sink);
		assert(sink.buffer == R"json({
    "a double": 3.1400000000000001,
    "a string": "This is a string",
    "an array": [
        "this",
        "is",
        "an",
        "array"
    ],
    "an integer": 123,
    "an object": {
        "key": "some value"
    }
})json");
	}

	void maikeKeyValueStoreObjectCreateInt()
	{
		Maike::KeyValueStore::Object test{123};
		assert(test.get().as<json_int_t>() == 123);
	}

	void maikeKeyValueStoreObjectCreateString()
	{
		Maike::KeyValueStore::Object test{"Foobar"};
		assert(std::string_view{"Foobar"} == test.get().as<char const*>());
	}

	void maikeKeyValueStoreObjectCreateDouble()
	{
		Maike::KeyValueStore::Object test{0.125};
		assert(test.get().as<double>() == 0.125);
	}

	void maikeKeyValueStoreObjectCreateArray()
	{
		std::vector<int> vals{1, 2, 3, 4};
		Maike::KeyValueStore::Object test{std::begin(vals), std::end(vals)};

		auto array = test.as<Maike::KeyValueStore::ArrayRefConst>();
		assert((std::equal(std::begin(array), std::end(array), std::begin(vals), [](auto a, auto b) {
			return Maike::KeyValueStore::get<json_int_t>(a) == b;
		})));
	}
}

int main()
{
	Testcases::maikeKeyValueStoreObjectCreateEmptySource();
	Testcases::maikeKeyValueStoreObjectCreateInvalidJson();
	Testcases::maikeKeyValueStoreObjectCreateFromJson();
	Testcases::maikeKeyValueStoreObjectGetHelperBuiltin();
	Testcases::maikeKeyValueStoreObjectGetHelperAdl();
	Testcases::maikeKeyValueStoreObjectGetHelperAdlInner();

	Testcases::maikeKeyValueStoreObjectNonExistingKey();
	Testcases::maikeKeyValueStoreObjectWrongType();

	Testcases::maikeKeyValueStoreObjectWrite();

	Testcases::maikeKeyValueStoreObjectCreateInt();
	Testcases::maikeKeyValueStoreObjectCreateString();
	Testcases::maikeKeyValueStoreObjectCreateDouble();
	Testcases::maikeKeyValueStoreObjectCreateArray();

	return 0;
}