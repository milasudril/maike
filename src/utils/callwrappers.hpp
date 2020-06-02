//@	{
//@	  "targets":[{"name":"callwrappers.hpp","type":"include"}]
//@	}

#ifndef MAIKE_UTILS_CALLWRAPPERS_HPP
#define MAIKE_UTILS_CALLWRAPPERS_HPP

#include <chrono>

namespace Maike
{
	template<class... T>
	void unusedResult(T&&...)
	{
	}

	template<class Function, class ... T>
	auto timedCall(Function&& f, T&& ... args)
	{
		auto now = std::chrono::steady_clock::now();
		auto result = f(std::forward<T>(args)...);
		auto t = std::chrono::steady_clock::now() - now;
		return std::make_pair(std::move(result), t);
	}
}

#endif
