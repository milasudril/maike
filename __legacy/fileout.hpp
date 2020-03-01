//@	{
//@	 "targets":[{"name":"fileout.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"fileout.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_FILEOUT_H
#define MAIKE_FILEOUT_H

#include "datasink.hpp"
#include "visibility.hpp"

namespace Maike
{
	class PRIVATE FileOut final: public DataSink
	{
	public:
		FileOut& operator=(const FileOut&) = delete;
		FileOut(const FileOut&) = delete;
		FileOut(FileOut&& file);
		FileOut& operator=(FileOut&& file);
		explicit FileOut(const char* filename);
		FileOut();

		enum class StdStream : unsigned int
		{
			OUTPUT,
			ERROR
		};
		FileOut(StdStream stream);
		~FileOut() noexcept;

		size_t write(const void* buffer, size_t count);

	private:
		intptr_t m_handle;
		void destroy()
		{
			delete this;
		}
	};
}

#endif
