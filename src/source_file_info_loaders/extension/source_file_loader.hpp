//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"./source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef EXTENSION_SOURCEFILELOADER_HPP
#define EXTENSION_SOURCEFILELOADER_HPP

#include "src/source_file_info_loaders/loader.hpp"

#include "src/key_value_store/array.hpp"

namespace Extension
{
	class SourceFileLoader
	{
	public:
		std::vector<Maike::Db::Dependency> getDependencies(Maike::Io::Reader) const
		{
			return std::vector<Maike::Db::Dependency>{};
		}

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
		                 Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const;

		static char const* name()
		{
			return "extension";
		}

		static Maike::Db::Compiler defaultCompiler()
		{
			return Maike::Db::Compiler{};
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
}

#endif