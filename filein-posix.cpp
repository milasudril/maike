//@	[
//@		[
//@		"posix"
//@			,{"targets":[{"name":"filein.o","type":"object"}]}
//@		]
//@	]

#include "filein.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include "strerror.hpp"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

using namespace Maike;

FileIn::FileIn(const char* filename):m_name(filename)
	{
	static_assert(sizeof(m_handle)>=sizeof(int),"Handle is too small");
	m_handle=open(filename,O_RDONLY);
	if(m_handle==-1)
		{
		exceptionRaise(ErrorMessage("It was not possible to open the file #0;. #1;"
			,{filename,static_cast<const char*>(strerror(errno))}));
		}
	}

FileIn::~FileIn()
	{
	fsync(static_cast<int>(m_handle));
	close(static_cast<int>(m_handle));
	}

size_t FileIn::read(void* buffer,size_t count)
	{
	auto pos=reinterpret_cast<uint8_t*>(buffer);
	size_t n_read=0;
	while(n_read!=count)
		{
		auto n=::read(static_cast<int>(m_handle),pos,count-n_read);
		if(n==0)
			{return n_read;}
		if(n==-1)
			{
			if(!( errno==EINTR || errno==EAGAIN || errno==EWOULDBLOCK))
				{exceptionRaise( ErrorMessage("I/O error",{}) );}
			n=0;
			}
		pos+=n;
		n_read+=n;
		}
	return n_read;
	}

