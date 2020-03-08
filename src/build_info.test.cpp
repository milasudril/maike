//@	{"targets":[{"name":"build_info.test.cpp","type":"application","autorun":1}]}

#undef NDEBUG

#include "./build_info.hpp"

#include <cassert>

namespace Testcases
{
	void maikeTypeIdDifferentId()
	{
		Maike::BuildId a;
		Maike::BuildId b;

		assert(a != b);
 	}
}

int main()
{
	Testcases::maikeTypeIdDifferentId();
	return 0;
}