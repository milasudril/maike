//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef PYTHON_SOURCEFILELOADER_HPP
#define PYTHON_SOURCEFILELOADER_HPP

#include "src/source_file_info_loaders/loader.hpp"

#include "src/key_value_store/array.hpp"

namespace Python
{
	class SourceFileLoader
	{
	public:
		SourceFileLoader() = default;

		explicit SourceFileLoader(Maike::KeyValueStore::CompoundRefConst)
		{
		}

		std::vector<Maike::Db::Dependency> getDependencies(Maike::Io::Reader) const
		{
			return std::vector<Maike::Db::Dependency>{};
		}

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
		                 Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const;

		Maike::Db::Compiler const& compiler() const
		{
			return m_compiler;
		}

		SourceFileLoader& compiler(Maike::Db::Compiler&&)
		{
			return *this;
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

	inline auto toJson(SourceFileLoader const&)
	{
		return Maike::KeyValueStore::Compound{}.takeHandle();
	}
}

#endif