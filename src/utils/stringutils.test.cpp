//@	 {"targets":[{"name":"stringutils.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./stringutils.hpp"

#include <cassert>

namespace Testcases
{
	void maikeStringutilsTrim()
	{
		assert(Maike::trim("      Foo bar") == "Foo bar");
		assert(Maike::trim("Foo bar      ") == "Foo bar");
		assert(Maike::trim("   Foo bar   ") == "Foo bar");
		assert(Maike::trim("             ") == "");
	}
}

int main()
{
	Testcases::maikeStringutilsTrim();
	return 0;
}
