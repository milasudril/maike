//@ {
//@ "targets":[ {"name":"sinkstd.o","type":"object"} ]
//@ }

#ifndef MAIKE_SINKSTD_H
#define MAIKE_SINKSTD_H

#include "datasink.hpp"

namespace Maike
	{
	namespace SinkStd
		{
		DataSink null;
		DataSink standard_error;
		DataSink standard_output;
		}
	};

#endif

