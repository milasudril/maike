//@ {
//@ "targets":[ {"name":"sinkstd.hpp","type":"include"} ]
//@	"dependencies_extra":[{"ref":"sinkstd.o","type":"implementation"}]
//@ }

#ifndef MAIKE_SINKSTD_H
#define MAIKE_SINKSTD_H

#include <cstddef>

namespace Maike
	{
	namespace SinkStd
		{
		extern DataSink null;
		extern DataSink standard_error;
		extern DataSink standard_output;
		}
	};

#endif

