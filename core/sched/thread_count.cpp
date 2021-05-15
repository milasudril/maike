//@	{
//@	  "targets":[{"name":"thread_count.o","type":"object"}]
//@	}

#include "./thread_count.hpp"

#include <cstring>
#include <cstdlib>
#include <stdexcept>

std::string Maike::ThreadCount::toString() const
{
	if(m_val < 0) { return "auto"; }

	return std::to_string(m_val);
}

Maike::ThreadCount::ThreadCount(char const* str)
{
	if(strcmp(str, "auto") == 0) { m_val = -1; }
	else
	{
		m_val = atoi(str);
		if(m_val < 1)
		{ throw std::runtime_error{"ThreadCount must be an integer greater than zero, or auto"}; }
	}
}
