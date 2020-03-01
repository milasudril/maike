//@	[
//@		[
//@		"and(not(less_than(linux,version('2.4'))),gnu)"
//@			,{
//@		 	"targets":[{"name":"directorylister.o","type":"object"}]
//@			}
//@		]
//@	]

#include "directorylister.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "strerror.hpp"
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <memory>

namespace
{
	struct linux_dirent
	{
		uint64_t d_ino;
		int64_t d_off;
		uint16_t d_reclen;
		uint8_t d_type;
		char d_name[];
	};

	constexpr size_t SIZE_BUFFER = 1024 * 1024;
}

using namespace Maike;

class DirectoryLister::Impl
{
public:
	Impl(const char* dirname): m_buffer(new uint8_t[SIZE_BUFFER])
	{
		fd = open(dirname, O_DIRECTORY | O_RDONLY);
		if(fd == -1)
		{
			exceptionRaise(ErrorMessage("It was not possible to open the directory #0;",
			                            {static_cast<const char*>(strerror(errno))}));
		}
		n_read = 0;
		r_pos_current = m_buffer.get();
	}

	~Impl() noexcept
	{
		close(fd);
	}

	const char* read()
	{
		if(n_read == 0)
		{
			r_pos_current = m_buffer.get();
			n_read = syscall(SYS_getdents64, fd, r_pos_current, SIZE_BUFFER);
			if(n_read == -1) { exceptionRaise(ErrorMessage("I/O error", {})); }
			if(n_read == 0) { return nullptr; }
		}

		auto data = reinterpret_cast<linux_dirent*>(r_pos_current);

		n_read -= data->d_reclen;
		r_pos_current += data->d_reclen;
		return data->d_name;
	}

private:
	std::unique_ptr<uint8_t[]> m_buffer;
	uint8_t* r_pos_current;
	ssize_t n_read;
	int fd;
};

DirectoryLister::DirectoryLister(const char* dirname): m_impl(new Impl(dirname))
{
}

DirectoryLister::~DirectoryLister() noexcept
{
	delete m_impl;
}

const char* DirectoryLister::read()
{
	return m_impl->read();
}
