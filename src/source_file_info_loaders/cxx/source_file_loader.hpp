//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_SOURCEFILELOADER_HPP
#define CXX_SOURCEFILELOADER_HPP

#include "src/source_file_info_loaders/loader.hpp"

namespace Cxx
{
	class SourceFileLoader
	{
	public:
		SourceFileLoader() = default;

		explicit SourceFileLoader(Maike::KeyValueStore::CompoundRefConst cfg):
		   m_compiler{cfg.get<Maike::Db::Compiler>("compiler")}
		{
		}

		std::vector<Maike::Db::Dependency> getDependencies(Maike::Io::Reader src) const;

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
		                 Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const;

		Maike::Db::Compiler getCompiler(Maike::KeyValueStore::CompoundRefConst) const
		{
			// TODO: Add configuration to new compiler instance before returning the compiler.
			return m_compiler;
		}

		Maike::Db::Compiler const& compiler() const
		{
			return m_compiler;
		}

	private:
		Maike::Db::Compiler m_compiler;
	};

	inline auto getDependencies(SourceFileLoader const& loader, Maike::Io::Reader src)
	{
		return loader.getDependencies(src);
	}

	inline auto filterInput(SourceFileLoader const& loader,
	                        Maike::Io::Reader input,
	                        Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
	                        Maike::SourceFileInfoLoaders::TagsOutStream tag_stream)
	{
		return loader.filterInput(input, source_stream, tag_stream);
	}

	inline auto getCompiler(SourceFileLoader const& loader, Maike::KeyValueStore::CompoundRefConst cfg)
	{
		return loader.getCompiler(cfg);
	}

	inline auto toJson(SourceFileLoader const& loader)
	{
		return Maike::KeyValueStore::Compound{}.set("compiler", loader.compiler()).takeHandle();
	}
}

#endif