//@	[
//@		[
//@		"posix"
//@			,{"targets":[{"name":"datasinkstd.o","type":"object"}]}
//@		]
//@	]

#include "datasinkstd.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include <errno.h>
#include <unistd.h>

using namespace Maike;

DataSinkNull DataSinkStd::null;

DataSinkStdHandle DataSinkStd::standard_error(STDERR_FILENO);
DataSinkStdHandle DataSinkStd::standard_output(STDOUT_FILENO);

DataSinkStdHandle::DataSinkStdHandle(intptr_t handle):r_handle(handle)
	{}

size_t DataSinkStdHandle::write(const void* buffer,size_t count)
	{
	if(count==0)
		{return 0;}
	auto bytes=reinterpret_cast<const uint8_t*>(buffer);
	size_t n=0;
	while(n!=count)
		{
		auto res=::write(static_cast<int>(r_handle),bytes,count - n);
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

DataSinkStdHandle::~DataSinkStdHandle()
	{
	fsync(static_cast<int>(r_handle));
	}


