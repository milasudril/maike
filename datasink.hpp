//@	{
//@	"targets":[ {"name":"datasink.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_DATASINK_HPP
#define MAIKE_DATASINK_HPP

#include <cstddef>

namespace Maike
	{
	class DataSink
		{
		public:
			virtual void write(const void* buffer,size_t n)=0;

		protected:
			~DataSink()=default;
		};
	};

#endif
