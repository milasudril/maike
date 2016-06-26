//@ {
//@ "targets":[ {"name":"datasinkstd.hpp","type":"include"} ]
//@	"dependencies_extra":[{"ref":"datasinkstd.o","type":"implementation"}]
//@ }

#ifndef MAIKE_DATASINKSTD_HPP
#define MAIKE_DATASINKSTD_HPP

#include "datasink.hpp"
#include <cstdint>

namespace Maike
	{
	class DataSinkStdHandle:public DataSink
		{
		public:
			DataSinkStdHandle(intptr_t x);
			~DataSinkStdHandle();
			void write(const void* buffer,size_t n);

		private:
			intptr_t r_handle;
		};


	namespace DataSinkStd
		{
		extern DataSink null;

		extern DataSinkStdHandle standard_error;
		extern DataSinkStdHandle standard_output;
		}
	};

#endif

