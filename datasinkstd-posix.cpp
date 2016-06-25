//@	[
//@		[
//@		"posix"
//@			,{"targets":[{"name":"datasinkstd.o","type":"object"}]}
//@		]
//@	]

#include "datasinkstd.hpp"
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
		black_magic(::write(static_cast<int>(r_handle),buffer,n) );
		}

	DataSinkStdHandle::~DataSinkStdHandle()
		{
		fsync(static_cast<int>(r_handle));
		}
	};


