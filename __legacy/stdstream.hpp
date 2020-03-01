//@	{
//@	 "targets":[{"name":"stdstream.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"stdstream.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_STDSTREAM_HPP
#define MAIKE_STDSTREAM_HPP

#include "visibility.hpp"

namespace Maike
{
	class DataSink;

	namespace StdStream
	{
		PRIVATE void errorSet(DataSink& stderr_new) noexcept;
		PRIVATE void outputSet(DataSink& stdout_new) noexcept;
		PRIVATE DataSink& error() noexcept;
		PRIVATE DataSink& output() noexcept;
	}
}

#endif
