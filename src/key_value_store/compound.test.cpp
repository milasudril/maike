//@	{
//@	  "targets":[{"name":"compound.test","type":"application","autorun":1}]
//@	 }

#undef NDEBUG

#include "./compound.hpp"

#include <cassert>

namespace Testcases
{
	void maikeKeyValueStoreCompoundCreate()
	{
		Maike::KeyValueStore::Compound c{};
		assert(c.size() == 0);
	}
}

int main()
{
	Testcases::maikeKeyValueStoreCompoundCreate();
}