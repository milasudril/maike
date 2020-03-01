//@	[
//@		[
//@		"and(not(less_than(linux,version('2.6.28'))),gnu)"
//@			,{"targets":[{"name":"timedscope.o","type":"object"}]}
//@		]
//@	]

#include "timedscope.hpp"
#include <time.h>

using namespace Maike;

TimedScope::TimedScope(double& T) noexcept: r_T(T)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
	r_T = static_cast<double>(t.tv_sec) + 1e-9 * static_cast<double>(t.tv_nsec);
}

TimedScope::~TimedScope() noexcept
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
	auto delta = (static_cast<double>(t.tv_sec) + 1e-9 * static_cast<double>(t.tv_nsec)) - r_T;
	r_T = delta;
}
