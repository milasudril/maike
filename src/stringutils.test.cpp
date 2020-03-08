//@	 {"targets":[{"name":"stringutils.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./stringutils.hpp"

#include <cassert>

namespace Testcases
{
	void maikeStringutilsToStringSysTime()
	{
		auto const val = std::chrono::system_clock::from_time_t(0);
		auto const str = Maike::toString(val);
		assert(str == "1970-01-01 0:00:00 UTC");
	}

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
	Testcases::maikeStringutilsToStringSysTime();
	Testcases::maikeStringutilsTrim();
}
