//@	{
//@	 "targets":[{"name":"system_time_stamp.o","type":"object"}]
//@	}

#include "./system_time_stamp.hpp"

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <array>

Maike::SystemTimeStamp::SystemTimeStamp(char const* str)
{
	char* end;
	auto l = strlen(str);
	errno = 0;
	auto tmp = strtoll(str, &end, 0);
	if(end != str + l || errno != 0)
	{ throw std::runtime_error{std::string{"Invalid timestamp "} + str}; }
	m_val = std::chrono::system_clock::from_time_t(tmp);
}

std::string Maike::format(SystemTimeStamp t)
{
	auto timeval = std::chrono::system_clock::to_time_t(t.value());

	tm time_decomposed;
	gmtime_r(&timeval, &time_decomposed);

	std::array<char, 64> buffer{};
	sprintf(buffer.data(),
	        "%04d-%02d-%02d %d:%02d:%02d UTC",
	        time_decomposed.tm_year + 1900,
	        time_decomposed.tm_mon + 1,
	        time_decomposed.tm_mday,
	        time_decomposed.tm_hour,
	        time_decomposed.tm_min,
	        time_decomposed.tm_sec);

	return std::string{buffer.data()};
}