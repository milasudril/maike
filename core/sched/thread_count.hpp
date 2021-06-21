//@	{
//@	 "dependencies_extra":[{"ref":"./thread_count.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_THREADCOUNT_HPP
#define MAIKE_THREADCOUNT_HPP

#include "core/utils/empty.hpp"

#include <sys/sysinfo.h>

#include <algorithm>
#include <string>

namespace Maike
{
	class ThreadCount
	{
	public:
		ThreadCount(): m_val{-1}
		{
		}

		explicit ThreadCount(char const* str);

		int value() const
		{
			return m_val < 0 ? std::max(get_nprocs(), 1) : m_val;
		}

		std::string toString() const;

	private:
		int m_val;
	};

	inline std::string toString(ThreadCount count)
	{
		return count.toString();
	}

	inline ThreadCount fromString(Empty<ThreadCount>, char const* str)
	{
		return ThreadCount{str};
	}

	constexpr char const* typeToString(Empty<ThreadCount>)
	{
		return "Thread count";
	}

	constexpr char const* typeDescription(Empty<ThreadCount>)
	{
		return "A *Thread count* is an integer that indicates how many threads to use. By default, the "
		       "number of threads is equal to the number of online logical processors.";
	}
}

#endif
