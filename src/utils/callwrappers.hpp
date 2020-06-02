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

	template<class T>
	struct TimedResult
	{
		TimedResult(T&& res, std::chrono::steady_clock::duration t): result{std::move(res)}, time{t}
		{
		}

		T result;
		std::chrono::steady_clock::duration time;
	};

	template<>
	struct TimedResult<void>
	{
		TimedResult(std::chrono::steady_clock::duration t): time{t}
		{
		}

		std::chrono::steady_clock::duration time;
	};

	template<class Function, class... T>
	auto timedCall(Function&& f, T&&... args)
	{
		auto now = std::chrono::steady_clock::now();
		if constexpr(std::is_same_v<std::invoke_result_t<Function, T...>, void>)
		{
			f(std::forward<T>(args)...);
			auto t = std::chrono::steady_clock::now() - now;
			return TimedResult<void>{t};
		}
		else
		{
			auto result = f(std::forward<T>(args)...);
			auto t = std::chrono::steady_clock::now() - now;
			return TimedResult{std::move(result), t};
		}
	}
}

#endif
