//@	{
//@	  "targets":[{"name":"json_handle.test.cpp","type":"application", "autorun": 1}]
//@	 }

#undef NDEBUG

#include "./json_handle.hpp"

#include <cassert>

namespace Testcases
{
	void maikeKeyValueStoreJsonHandleCreate()
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
}

int main()
{
	Testcases::maikeKeyValueStoreJsonHandleCreate();
	return 0;
}
