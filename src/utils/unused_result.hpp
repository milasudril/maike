//@	{
//@	  "targets":[{"name":"unused_result.hpp","type":"include"}]
//@	}

#ifndef MAIKE_UTILS_UNUSEDRESULT_HPP
#define MAIKE_UTILS_UNUSEDRESULT_HPP

namespace Maike
{
	template<class... T>
	void unusedResult(T&&...)
	{
	}
}

#endif
