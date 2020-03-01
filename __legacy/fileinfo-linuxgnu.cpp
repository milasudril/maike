//@	[
//@		[
//@		"and(not(less_than(linux,version('2.6'))),gnu)"
//@			,{"targets":[{"name":"fileinfo.o","type":"object"}]}
//@		]
//@	]

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "fileinfo.hpp"
#include "variant.hpp"
#include "errormessage.hpp"
#include "strerror.hpp"
#include "exceptionhandler.hpp"
#include <errno.h>
#include <sys/stat.h>

using namespace Maike;

FileInfo::FileInfo(const char* filename)
{
	struct stat info;
	if(stat(filename, &info) == -1)
	{
		exceptionRaise(ErrorMessage("It was not possible to retrieve information about \"#0;\". #1;",
		                            {filename, static_cast<const char*>(strerror(errno))}));
	}

	m_time_modified =
	   static_cast<double>(info.st_mtim.tv_sec) + 1e9 * static_cast<double>(info.st_mtim.tv_nsec);

	m_time_accessed =
	   static_cast<double>(info.st_atim.tv_sec) + 1e9 * static_cast<double>(info.st_atim.tv_nsec);

	static_assert(sizeof(off_t) >= 8, "Your system does not support large files");
	m_size = info.st_size;
	m_type = Type::OTHER;
	if(S_ISDIR(info.st_mode)) { m_type = Type::DIRECTORY; }
	if(S_ISREG(info.st_mode)) { m_type = Type::FILE; }
}
