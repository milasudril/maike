//@	{
//@	  "targets":[{"name":"callwrappers.hpp","type":"include"}]
//@	}

#ifndef MAIKE_UTILS_CALLWRAPPERS_HPP
#define MAIKE_UTILS_CALLWRAPPERS_HPP

namespace Maike
{
	template<class... T>
	void unusedResult(T&&...)
	{
	}
}

#endif
