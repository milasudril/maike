//@	{"targets":[{"name":"build_info.test.cpp","type":"application","autorun":1}]}

#undef NDEBUG

#include "./build_info.hpp"

#include <cassert>
#include <algorithm>

namespace Testcases
{
	void maikeTypeIdDifferentId()
	{
		Maike::BuildId a;
		Maike::BuildId b;

		assert(a != b);
 	}

	void maikeTypeIdToString()
	{
		std::array<std::byte, 32> bits;

		std::generate(std::begin(bits), std::end(bits), [k = 0]() mutable {
			return static_cast<std::byte>(k++);
		});

		Maike::BuildId a{bits};

		assert(toString(a) == "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F");
 	}



}

int main()
{
	Testcases::maikeTypeIdDifferentId();
	Testcases::maikeTypeIdToString();
	return 0;
}