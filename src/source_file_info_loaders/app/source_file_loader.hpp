//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef APP_SOURCEFILELOADER_HPP
#define APP_SOURCEFILELOADER_HPP

#include "src/source_file_info_loaders/loader.hpp"

namespace App
{
	class SourceFileLoader
	{
	public:
		SourceFileLoader(): m_compiler{"cxx_compiler", ""}
		{
		}

#if 0
		explicit SourceFileLoader(Maike::KeyValueStore::CompoundRefConst)
		//	TODO:	   m_compiler{cfg.get<Compiler>("compiler")}
		{
		}
#endif

		std::vector<Maike::Db::Dependency> getDependencies(Maike::Io::Reader) const
		{
			return std::vector<Maike::Db::Dependency>{};
		}

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
		                 Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const;

		Maike::Db::Compiler getCompiler(Maike::KeyValueStore::CompoundRefConst) const
		{
			return m_compiler;
		}
#if 0
		Compiler const& compiler() const
		{
			return m_compiler;
		}
#endif

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

	inline auto toJson(SourceFileLoader const&)
	{
		return Maike::KeyValueStore::Compound{}
		   .takeHandle(); //.set("compiler", loader.compiler()).takeHandle();
	}
}

#endif