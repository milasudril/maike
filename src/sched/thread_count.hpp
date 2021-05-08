//@	{
//@	 "dependencies_extra":[{"ref":"./thread_count.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_THREADCOUNT_HPP
#define MAIKE_THREADCOUNT_HPP

#include "src/utils/empty.hpp"

#include <sys/sysinfo.h>

#include <algorithm>

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
		return "ThreadCount";
	}
}

#endif
