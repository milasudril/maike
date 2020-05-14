//@	{
//@	 "targets":[{"name":"system_time_stamp.o","type":"object"}]
//@	}

#include "./system_time_stamp.hpp"

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>

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