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
		auto const n = std::min(std::min(static_cast<size_t>(1), N), src.n_bytes_left);
		if (n != 0)
		{
			*buffer = static_cast<std::byte>(*src.read_ptr);
		}
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
	void maikeKeyValueStoreJsonHandleCreateAndMove()
	{
		Maike::KeyValueStore::JsonHandle h1{};

		assert(!h1.valid());

		Maike::KeyValueStore::JsonHandle h2{json_object()};
		assert(h2.valid());

		auto h2ptr = h2.get();
		h1 = std::move(h2);
		assert(!h2.valid());
		assert(h1.get() == h2ptr);

		Maike::KeyValueStore::JsonHandle other{std::move(h1)};
		assert(!h1.valid());
		assert(other.get() == h2ptr);
	}

	void maikeKeyValueStoreJsonHandleLoadEmpty()
	{
		auto val = Maike::KeyValueStore::jsonLoad(EmptySource{});
		assert(!val.valid());
	}

	void maikeKeyValueStoreJsonHandleLoadJunk()
	{
		try
		{
			auto val = Maike::KeyValueStore::jsonLoad(StringViewSource{R"json(
{"invalid json": "missing curly brace")json"});
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
		auto val = Maike::KeyValueStore::jsonLoad(src);
		assert(*src.read_ptr == '\n');
	}
}

int main()
{
	Testcases::maikeKeyValueStoreJsonHandleCreateAndMove();
	Testcases::maikeKeyValueStoreJsonHandleLoadEmpty();
	Testcases::maikeKeyValueStoreJsonHandleLoadJunk();
	Testcases::maikeKeyValueStoreJsonHandleLoadDataAfterJson();

	return 0;
}
