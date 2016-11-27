//@	{
//@	 "targets":[{"name":"hashlong.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"hashlong.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_HASHLONG_HPP
#define MAIKE_HASHLONG_HPP

#include <array>

namespace Maike
	{
	class DataSource;
	std::array<uint8_t,32> hashlong(DataSource&& src);
	}

#endif
