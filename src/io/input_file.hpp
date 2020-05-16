//@	{
//@	 "targets":[{"name":"input_file.hpp","type":"include"}]
//@	}

#ifndef MAIKE_INPUTFILE_HPP
#define MAIKE_INPUTFILE_HPP

#include "src/fs.hpp"

#include <unistd.h>
#include <fcntl.h>

#include <utility>

namespace Maike
{
	class InputFile
	{
	public:
		explicit InputFile(fs::path const& path): m_handle{open(path.c_str(), O_RDONLY)}
		{
		}

		InputFile(InputFile&& other): m_handle{other.m_handle}
		{
			other.m_handle = -1;
		}

		InputFile& operator=(InputFile&& other)
		{
			std::swap(other.m_handle, m_handle);
			other.reset();
			return *this;
		}

		~InputFile()
		{
			reset();
		}

		size_t read(std::byte* buffer, size_t n)
		{
			return ::read(m_handle, buffer, n);
		}

		void reset()
		{
			if(m_handle != -1) { close(m_handle); }
		}

		int get() const
		{
			return m_handle;
		}

	private:
		int m_handle;
	};

	inline size_t read(InputFile& src, std::byte* buffer, size_t n)
	{
		return src.read(buffer, n);
	}
}

#endif