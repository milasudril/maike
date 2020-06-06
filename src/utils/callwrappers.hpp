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

	template<class OnCompleted, class Function, class... T>
	decltype(auto) timedCall(OnCompleted&& task_completed, Function&& f, T&&... args)
	{
		auto now = std::chrono::steady_clock::now();
		if constexpr(std::is_same_v<std::invoke_result_t<Function, T...>, void>)
		{
			f(std::forward<T>(args)...);
			task_completed(std::chrono::steady_clock::now() - now);
		}
		else
		{
			auto result = f(std::forward<T>(args)...);
			task_completed(std::chrono::steady_clock::now() - now, result);
			return result;
		}
	}
}

#endif
