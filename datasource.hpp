//@	{
//@	"targets":[ {"name":"datasource.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_DATASOURCE_HPP
#define MAIKE_DATASOURCE_HPP

#include <cstddef>

namespace Maike
	{
	class DataSource
		{
		public:
			virtual size_t read(void* buffer,size_t n)=0;
			virtual const char* nameGet() const noexcept=0;

		protected:
			~DataSource()=default;
		};
	};

#endif
