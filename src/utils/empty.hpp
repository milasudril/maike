//@	{
//@	  "targets":[{"name":"empty.hpp","type":"include"}]
//@	}

#ifndef MAIKE_EMPTY_HPP
#define MAIKE_EMPTY_HPP

#include <type_traits>

namespace Maike
{
	template<class T>
	struct Empty
	{
		using type = T;
	};
}

#endif