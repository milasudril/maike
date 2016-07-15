//@	{"targets":[{"name":"writebuffer.o","type":"object"}]}

#include "writebuffer.hpp"
#include "stringformat.hpp"

using namespace Maike;

WriteBuffer& WriteBuffer::write(const char* str)
	{
	auto begin=str;
	while(*str!='\0')
		{
		write(static_cast<uint8_t>(*str));
		++str;
		}
	if(begin!=str)
		{
		if(*(str - 1)=='\n') //flush when last charater is linefeed
			{flush();}
		}
	return *this;
	}

WriteBuffer& WriteBuffer::write(long long int x)
	{
	char buffer[32];
	format({buffer,buffer + 32},"#0;",{static_cast<int64_t>(x)});
	return write(buffer);
	}


WriteBuffer& WriteBuffer::write(size_t x)
	{
	char buffer[32];
	format({buffer,buffer + 32},"#0;",{static_cast<uint64_t>(x)});
	return write(buffer);
	}

WriteBuffer& WriteBuffer::write(double x)
	{
	char buffer[32];
	format({buffer,buffer + 32},"#0;",{x});
	return write(buffer);
	}
