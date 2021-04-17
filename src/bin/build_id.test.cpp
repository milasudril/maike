//@	{"targets":[{"name":"build_id.test","type":"application","autorun":1}]}

#undef NDEBUG

#include "./build_id.hpp"

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

		std::generate(
		   std::begin(bits), std::end(bits), [k = 0]() mutable { return static_cast<std::byte>(k++); });

		Maike::BuildId a{bits};

		assert(toString(a) == "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
	}

	void maikeTypeIdFromString()
	{
		Maike::BuildId id{"000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"};

		assert(toString(id) == "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
	}
}

int main()
{
	Testcases::maikeTypeIdDifferentId();
	Testcases::maikeTypeIdToString();
	Testcases::maikeTypeIdFromString();
	return 0;
}