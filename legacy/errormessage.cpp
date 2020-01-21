//@	{"targets":[{"name":"errormessage.o","type":"object"}]}

#include "errormessage.hpp"
#include "stringformat.hpp"

using namespace Maike;

ErrorMessage::ErrorMessage(const char* format_string
	,const std::initializer_list<Variant>& args) noexcept
	{
	format({m_buffer,m_buffer+length()},format_string,args);
	}
