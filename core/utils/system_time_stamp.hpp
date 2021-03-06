//@	{
//@	"dependencies_extra":[{"ref":"./system_time_stamp.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TIME_SYSTEMTIMESTAMP_HPP
#define MAIKE_TIME_SYSTEMTIMESTAMP_HPP

#include "./empty.hpp"

#include <chrono>
#include <string>

namespace Maike
{
	class SystemTimeStamp
	{
	public:
		SystemTimeStamp(): m_val{std::chrono::system_clock::now()}
		{
		}

		explicit SystemTimeStamp(int64_t val): m_val{std::chrono::system_clock::from_time_t(val)}
		{
		}

		explicit SystemTimeStamp(char const* str);

		auto value() const
		{
			return m_val;
		}

	private:
		std::chrono::system_clock::time_point m_val;
	};

	std::string format(SystemTimeStamp val);

	inline SystemTimeStamp fromString(Empty<SystemTimeStamp>, char const* str)
	{
		return SystemTimeStamp{str};
	}

	constexpr char const* typeToString(Empty<SystemTimeStamp>)
	{
		return "Timestamp";
	}

	constexpr char const* typeDescription(Empty<SystemTimeStamp>)
	{
		return "A *Timestamp* is an integer indicates how many seconds that has passed since UNIX epoch. "
		       "By default, the current system time in is used.";
	}
}
#endif
