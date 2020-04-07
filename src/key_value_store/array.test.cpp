//@	{
//@	  "targets":[{"name":"array.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./array.hpp"

#include <cassert>
#include <algorithm>
#include <cstring>

namespace
{
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
}

namespace Testcases
{
	void maikeKeyValueStoreArrayCreateAndInsertValues()
	{
		Maike::KeyValueStore::Array array{};
		assert(array.size() == 0);
		assert(array.begin() == array.end());

		array.append(0).append(1).append(2).append(Maike::KeyValueStore::Array{}.append(3).append(4));

		std::for_each(std::begin(array), std::end(array), [k = 0](auto item) mutable {
			if(k < 3) { assert(Maike::KeyValueStore::get<json_int_t>(item) == k); }
			else
			{
				auto array = Maike::KeyValueStore::get<Maike::KeyValueStore::ArrayRefConst>(item);
				assert(array.size() == 2);
				auto i = array.begin();
				assert(Maike::KeyValueStore::get<json_int_t>(*i) == 3);
				++i;
				assert(Maike::KeyValueStore::get<json_int_t>(*i) == 4);
			}
			++k;
		});
	}

	void maikeKeyValueStoreArrayCreateAndTakeHandle()
	{
		Maike::KeyValueStore::Array array{};
		array.append(0).append(1);
		assert(array.size() == 2);

		auto h = array.takeHandle();
		assert(h.valid());
		assert(array.size() == 0);
	}

	void maikeKeyValueStoreArrayLoad()
	{
		Maike::KeyValueStore::Array array{StringViewSource{"[1, 2, 3]"}, ""};
		assert(array.size() == 3);
	}

	void maikeKeyValueStoreArrayLoadWrongType()
	{
		try
		{
			Maike::KeyValueStore::Array array{StringViewSource{"{\"foo\": 1123}"}, ""};
			abort();
		}
		catch(...)
		{
		}
	}
}

int main()
{
	Testcases::maikeKeyValueStoreArrayCreateAndInsertValues();
	Testcases::maikeKeyValueStoreArrayCreateAndTakeHandle();
	Testcases::maikeKeyValueStoreArrayLoad();
	Testcases::maikeKeyValueStoreArrayLoadWrongType();
}