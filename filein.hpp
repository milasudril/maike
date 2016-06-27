//@	{
//@	 "targets":[{"name":"filein.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filein.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_FILEIN_H
#define MAIKE_FILEIN_H

#include "datasource.hpp"
#include <string>

namespace Maike
	{
	class FileIn:public DataSource
		{
		public:
			explicit FileIn(const char* filename);
			~FileIn();

			size_t read(void* buffer,size_t count);

			const char* nameGet() const noexcept
				{return m_name.c_str();}

		private:
			std::string m_name;
			intptr_t m_handle;
		};
	}

#endif
