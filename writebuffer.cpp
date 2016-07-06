//@	{"targets":[{"name":"writebuffer.o","type":"object"}]}

#include "writebuffer.hpp"

using namespace Maike;

WriteBuffer& WriteBuffer::write(const char* str)
	{
	auto begin=str;
	while(*str!='\0')
		{
		write(*str);
		++str;
		}
	if(begin!=str)
		{
		if(*(str - 1)=='\n') //flush when last charater is linefeed
			{flush();}
		}
	return *this;
	}
