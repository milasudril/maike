//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_SOURCEFILELOADER_HPP
#define CXX_SOURCEFILELOADER_HPP

#include "src/source_tree_loader/source_file_loader.hpp"

namespace Cxx
{
	class SourceFileLoader
	{
	public:
		std::vector<Maike::Dependency> getDependencies(Maike::Reader src) const;

		void filterInput(Maike::Reader input,
		                 Maike::SourceTreeLoader::SourceOutStream source_stream,
		                 Maike::SourceTreeLoader::TagsOutStream tag_stream) const;

		Maike::Compiler getCompiler(Maike::KeyValueStore::CompoundRefConst) const
		{
			return Maike::Compiler{};
		}
	};

	inline auto getDependencies(SourceFileLoader const& loader, Maike::Reader src)
	{
		return loader.getDependencies(src);
	}

	inline auto filterInput(SourceFileLoader const& loader,
	                        Maike::Reader input,
	                        Maike::SourceTreeLoader::SourceOutStream source_stream,
	                        Maike::SourceTreeLoader::TagsOutStream tag_stream)
	{
		return loader.filterInput(input, source_stream, tag_stream);
	}

	inline auto getCompiler(SourceFileLoader const& loader, Maike::KeyValueStore::CompoundRefConst cfg)
	{
		return loader.getCompiler(cfg);
	}
}

#endif