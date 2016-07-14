//@	{
//@	 "targets":[{"name":"fileout.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"fileout.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_FILEOUT_H
#define MAIKE_FILEOUT_H

#include "datasink.hpp"
namespace Maike
	{
	class FileOut final:public DataSink
		{
		public:
			FileOut& operator=(const FileOut&)=delete;
			FileOut(const FileOut& )=delete;
			FileOut(FileOut&& file);
			FileOut& operator=(FileOut&& file);
			explicit FileOut(const char* filename);
			FileOut();
			~FileOut();

			size_t write(const void* buffer,size_t count);

		private:
			intptr_t m_handle;
			void destroy()
				{delete this;}
		};
	}

#endif
