//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_SOURCEFILELOADER_HPP
#define CXX_SOURCEFILELOADER_HPP

#include "./compiler.hpp"

#include "src/source_tree_loader/source_file_loader.hpp"

namespace Cxx
{
	class SourceFileLoader
	{
	public:
		std::vector<Maike::Dependency> getDependencies(Maike::Io::Reader src) const;

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceTreeLoader::SourceOutStream source_stream,
		                 Maike::SourceTreeLoader::TagsOutStream tag_stream) const;

		Maike::Compiler getCompiler(Maike::KeyValueStore::CompoundRefConst) const
		{
			return Maike::Compiler{m_compiler};
		}

	private:
		Compiler m_compiler;
	};

	inline auto getDependencies(SourceFileLoader const& loader, Maike::Io::Reader src)
	{
		return loader.getDependencies(src);
	}

	inline auto filterInput(SourceFileLoader const& loader,
	                        Maike::Io::Reader input,
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