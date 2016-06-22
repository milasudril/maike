//@ {
//@ "targets":[ {"name":"datasink.hpp","type":"include"} ]
//@ }

#ifndef MAIKE_DATASINK_H
#define MAIKE_DATASINK_H

#include <cstddef>

namespace Maike
	{
	class DataSink
		{
		public:
			virtual void write(const void* buffer,size_t n){}
		};
	};

#endif
