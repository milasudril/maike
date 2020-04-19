//@	{
//@	 "targets":[{"name":"tag_filter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"tag_filter.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_CXX_TAGFILTER_HPP
#define MAIKE_CXX_TAGFILTER_HPP

#include "src/tag_filter.hpp"

namespace Maike::Cxx
{
	class TagFilter
	{
	public:
		void run(Reader input, SourceOutStream source_stream, TagsOutStream tag_stream) const;
	};

	inline void
	run(TagFilter const& filter, Reader input, SourceOutStream source_stream, TagsOutStream tag_stream)
	{
		filter.run(input, source_stream, tag_stream);
	}
}

#endif