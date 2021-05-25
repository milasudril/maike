//@	{
//@	 "dependencies_extra":[{"ref":"./mem_status.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_UTILS_MEMMONITOR_HPP
#define MAIKE_UTILS_MEMMONITOR_HPP

#include <chrono>
#include <condition_variable>
#include <thread>

namespace Maike
{
	size_t getMemAvail();
}

#endif
