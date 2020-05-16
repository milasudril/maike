//@	{
//@	 "targets":[{"name":"system_time_stamp.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"system_time_stamp.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SYSTEMTIMESTAMP_HPP
#define MAIKE_SYSTEMTIMESTAMP_HPP

#include "src/utils/empty.hpp"

#include <chrono>

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

	inline SystemTimeStamp fromString(Empty<SystemTimeStamp>, char const* str)
	{
		return SystemTimeStamp{str};
	}

	constexpr char const* typeToString(Empty<SystemTimeStamp>)
	{
		return "timestamp";
	}

}
#endif
