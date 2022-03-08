//@	{
//@	"dependencies_extra":[{"ref":"./source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_SOURCEFILELOADER_HPP
#define CXX_SOURCEFILELOADER_HPP

#include "core/source_file_info_loaders/loader.hpp"

#include "core/key_value_store/array.hpp"

#include <regex>

namespace Cxx
{
	class SourceFileLoader
	{
	public:
		SourceFileLoader():SourceFileLoader{{".*\\.gen\\.hpp$"}}{}

		explicit SourceFileLoader(std::vector<std::string>&& gen_includes_src):
		   m_gen_includes_src{std::move(gen_includes_src)}
		{
			std::transform(std::begin(m_gen_includes_src),
			               std::end(m_gen_includes_src),
			               std::back_inserter(m_gen_includes),
			               [](auto const& item) {
				               return std::regex{item.c_str(), std::regex_constants::basic};
			               });
		}

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

		std::vector<std::string> const& generated_includes_src() const
		{
			return m_gen_includes_src;
		}

		std::vector<std::regex> const& generated_includes() const { return m_gen_includes; }

	private:
		std::vector<std::regex> m_gen_includes;
		std::vector<std::string> m_gen_includes_src;
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

	inline Maike::KeyValueStore::JsonHandle toJson(SourceFileLoader const& loader)
	{
		auto const& gen_includes = loader.generated_includes_src();

		return Maike::KeyValueStore::Compound{}
		   .set("generated_includes",
		        Maike::KeyValueStore::Array{std::begin(gen_includes), std::end(gen_includes)})
		   .takeHandle();
	}

	bool useTargetDeps(SourceFileLoader const&, Maike::Db::Compiler const& compiler);

	inline SourceFileLoader fromJson(Maike::KeyValueStore::Empty<SourceFileLoader>,
	                                 Maike::KeyValueStore::JsonRefConst ref)
	{
		auto obj = ref.as<Maike::KeyValueStore::CompoundRefConst>();
		std::vector<std::string> gen_includes_extracted;
		if(auto gen_includes = obj.getIf<Maike::KeyValueStore::ArrayRefConst>("generated_includes");
		   gen_includes.has_value())
		{
			std::transform(std::begin(*gen_includes),
			               std::end(*gen_includes),
			               std::back_inserter(gen_includes_extracted),
			               [](auto const& item) { return item.template as<char const*>(); });
		}

		return SourceFileLoader{std::move(gen_includes_extracted)};
	}
}

#endif
