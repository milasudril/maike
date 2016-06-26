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

namespace Maike
	{
	namespace DataSinkStd
		{
		DataSink null;

		DataSinkStdHandle standard_error(STDERR_FILENO);
		DataSinkStdHandle standard_output(STDOUT_FILENO);
		}

	DataSinkStdHandle::DataSinkStdHandle(intptr_t handle):r_handle(handle)
		{}

	inline void black_magic(ssize_t x)
		{}

	void DataSinkStdHandle::write(const void* buffer,size_t n)
		{
		auto bytes=reinterpret_cast<const uint8_t*>(buffer);
		while(n)
			{
			auto res=::write(static_cast<int>(r_handle),bytes,n);
			if(res==-1)
				{
				if(errno!=EINTR)
					{exceptionRaise(ErrorMessage("I/O error",{}));}
				}
			n-=res;
			bytes+=n;
			}
		}

	DataSinkStdHandle::~DataSinkStdHandle()
		{
		fsync(static_cast<int>(r_handle));
		}
	};


