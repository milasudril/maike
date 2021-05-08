//@	{"targets":[{"name":"with_mutex.hpp", "type":"include"}]}

#ifndef MAIKE_UTILS_WITHMUTEX_HPP
#define MAIKE_UTILS_WITHMUTEX_HPP

#include <functional>

namespace Maike
{
	template<template<class> class LockType, class Mutex, class Func, class... Args>
	auto invokeWithMutex(Mutex& mtx, Func&& f, Args&&... args)
	{
		LockType lock{mtx};
		return std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
	}
}

#endif
