//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_CXX_SOURCEFILELOADER_HPP
#define MAIKE_CXX_SOURCEFILELOADER_HPP

#include "src/source_file_loader/source_file_loader.hpp"

namespace Maike::Cxx
{
	class SourceFileLoader
	{
	public:
		std::vector<Dependency> getDependencies(Reader src) const;

		void filterInput(Reader input, SourceOutStream source_stream, TagsOutStream tag_stream) const;

		Compiler getCompiler(KeyValueStore::CompoundRefConst) const
		{
			return Compiler{};
		}
	};

	inline auto getDependencies(SourceFileLoader const& loader, Reader src)
	{
		return loader.getDependencies(src);
	}

	inline auto filterInput(SourceFileLoader const& loader,
	                        Reader input,
	                        SourceOutStream source_stream,
	                        TagsOutStream tag_stream)
	{
		return loader.filterInput(input, source_stream, tag_stream);
	}

	inline auto getCompiler(SourceFileLoader const& loader, KeyValueStore::CompoundRefConst cfg)
	{
		return loader.getCompiler(cfg);
	}
}

#endif