//@	{
//@	  "targets":[{"name":"json_handle.test","type":"application", "autorun": 1}]
//@	 }

#undef NDEBUG

#include "./json_handle.hpp"

#include <cassert>
#include <cstring>

namespace
{
	struct EmptySource
	{
	};

	size_t read(EmptySource, std::byte*, size_t)
	{
		return 0;
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

	struct Sink
	{
		std::string m_value;
	};

	void write(Sink& sink, char const* buffer, size_t n)
	{
		sink.m_value.insert(std::end(sink.m_value), buffer, buffer + n);
	}

}

namespace Testcases
{
	void maikeKeyValueStoreJsonHandleCreateAndMove()
	{
		Maike::KeyValueStore::JsonHandle h1{};

		assert(!h1.valid());

		Maike::KeyValueStore::JsonHandle h2{json_object()};
		assert(h2.valid());

		auto h2ptr = h2.handle();
		h1 = std::move(h2);
		assert(!h2.valid());
		assert(h1.handle() == h2ptr);

		Maike::KeyValueStore::JsonHandle other{std::move(h1)};
		assert(!h1.valid());
		assert(other.handle() == h2ptr);
	}

	void maikeKeyValueStoreJsonHandleLoadEmpty()
	{
		auto val = Maike::KeyValueStore::jsonLoad(EmptySource{}, "");
		assert(!val.valid());
	}

	void maikeKeyValueStoreJsonHandleLoadJunk()
	{
		try
		{
			auto val = Maike::KeyValueStore::jsonLoad(StringViewSource{R"json(
{"invalid json": "missing curly brace")json"},
			                                          "");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeKeyValueStoreJsonHandleLoadDataAfterJson()
	{
		StringViewSource src{R"json(
{"Key": "value"}
Here is some junk)json"};
		auto val = Maike::KeyValueStore::jsonLoad(src, "");
		assert(val.valid());
	}

	void maikeKeyValueStoreJsonHandleLoadWhitespaceOnly()
	{
		StringViewSource src{
		   "                         \n"
		   "                                              \n"
		   "                                              "};
		auto val = Maike::KeyValueStore::jsonLoad(src, "");
		assert(!val.valid());
	}

	void maikeKeyValueStoreJsonHandleLoadStore()
	{
		auto val = Maike::KeyValueStore::jsonLoad(StringViewSource{R"json({
"an object": {"key": "some value"},
"an array": ["this", "is", "an", "array"],
"a string": "This is a string",
"an integer": 123,
"a double": 3.14
})json"},
		                                          "");

		Sink s;
		store(val, s);

		assert(R"json({
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
})json" == s.m_value);
	}

	void maikeKeyValueStoreJsonHandleGetValuesCorrectType()
	{
		{
			Maike::KeyValueStore::JsonHandle h{"Foobar"};
			assert(std::string_view{get(Maike::KeyValueStore::Empty<char const*>{}, h)} == "Foobar");
		}

		{
			Maike::KeyValueStore::JsonHandle h{123};
			assert(get(Maike::KeyValueStore::Empty<json_int_t>{}, h) == 123);
		}

		{
			Maike::KeyValueStore::JsonHandle h{0.125};
			assert(get(Maike::KeyValueStore::Empty<double>{}, h) == 0.125);
		}
	}

	void maikeKeyValueStoreJsonHandleGetValueWrongType()
	{
		try
		{
			Maike::KeyValueStore::JsonHandle h{"Foobar"};
			(void)get(Maike::KeyValueStore::Empty<json_int_t>{}, h);
			abort();
		}
		catch(...)
		{
		}
	}
}

int main()
{
	Testcases::maikeKeyValueStoreJsonHandleCreateAndMove();
	Testcases::maikeKeyValueStoreJsonHandleLoadEmpty();
	Testcases::maikeKeyValueStoreJsonHandleLoadJunk();
	Testcases::maikeKeyValueStoreJsonHandleLoadDataAfterJson();
	Testcases::maikeKeyValueStoreJsonHandleLoadWhitespaceOnly();
	Testcases::maikeKeyValueStoreJsonHandleLoadStore();
	Testcases::maikeKeyValueStoreJsonHandleGetValuesCorrectType();
	Testcases::maikeKeyValueStoreJsonHandleGetValueWrongType();

	return 0;
}
