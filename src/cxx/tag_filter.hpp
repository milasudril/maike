//@	{
//@	 "targets":[{"name":"tag_filter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"tag_filter.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_CXX_TAGFILTER_HPP
#define MAIKE_CXX_TAGFILTER_HPP

#include "src/reader.hpp"
#include "src/writer.hpp"

namespace Maike::Cxx
{
	class TagFilter
	{
	public:
		void run(Reader src, Writer tag_stream, Writer source_stream) const;
	};
}

#endif