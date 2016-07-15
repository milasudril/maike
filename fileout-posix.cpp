//@	[
//@		[
//@		"posix"
//@			,{"targets":[{"name":"fileout.o","type":"object"}]}
//@		]
//@	]

#include "fileout.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include "strerror.hpp"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

using namespace Maike;

FileOut::FileOut(const char* filename)
	{
	static_assert(sizeof(m_handle)>=sizeof(int),"Handle is too small");
	m_handle=open(filename,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
	if(m_handle==-1)
		{
		exceptionRaise(ErrorMessage("It was not possible to create the file #0;. #1;"
			,{filename,static_cast<const char*>(strerror(errno))}));
		}
	}

FileOut::FileOut(FileOut&& file):m_handle(file.m_handle)
	{
	file.m_handle=-1;
	}

FileOut& FileOut::operator=(FileOut&& file)
	{
	std::swap(m_handle,file.m_handle);
	return *this;
	}

FileOut::FileOut():m_handle(STDOUT_FILENO)
	{}

FileOut::~FileOut()
	{
	if(m_handle!=STDOUT_FILENO)
		{
		fsync(static_cast<int>(m_handle));
		close(static_cast<int>(m_handle));
		}
	}

size_t FileOut::write(const void* buffer,size_t count)
	{
	if(count==0)
		{return 0;}
	auto bytes=reinterpret_cast<const uint8_t*>(buffer);
	size_t n=0;
	while(n!=count)
		{
		auto res=::write(static_cast<int>(m_handle),bytes,count - n);
		if(res==-1)
			{
			if(errno==ENOSPC)
				{return n;}
			if(!( errno==EINTR || errno==EAGAIN || errno==EWOULDBLOCK))
				{exceptionRaise(ErrorMessage("I/O error",{}));}
			n=0;
			}
		n+=res;
		bytes+=res;
		}
	return n;
	}
