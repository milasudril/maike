//@	{
//@	"dependencies_extra":[{"ref":"./source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_SOURCEFILELOADER_HPP
#define CXX_SOURCEFILELOADER_HPP

#include "core/source_file_info_loaders/loader.hpp"

#include "core/key_value_store/array.hpp"

namespace Cxx
{
	class SourceFileLoader
	{
	public:
		std::vector<Maike::Db::Dependency> getDependencies(Maike::Io::Reader src) const;

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
		                 Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const;

		static Maike::Db::Compiler defaultCompiler()
		{
			return Maike::Db::Compiler{
			   "cxx_compiler.py",
			   Maike::KeyValueStore::Compound{}
			      .set("iquote", Maike::KeyValueStore::Array{}.append("."))
			      .set("cflags", Maike::KeyValueStore::Array{}.append("-g").append("-Wall")),
			   false};
		}

		static char const* name()
		{
			return "cxx_src_loader";
		}
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

	inline Maike::KeyValueStore::JsonHandle toJson(SourceFileLoader const&)
	{
		return Maike::KeyValueStore::Compound{}.takeHandle();
	}

	bool useTargetDeps(SourceFileLoader const&, Maike::Db::Compiler const& compiler);
}

#endif
