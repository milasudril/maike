//@	{"dependencies_extra":[{"ref":"./source_file_loader.o", "rel":"implementation"}]}

#ifndef GENERIC_SOURCEFILELOADER_HPP
#define GENERIC_SOURCEFILELOADER_HPP

#include "core/source_file_info_loaders/loader.hpp"

#include "core/key_value_store/compound.hpp"

namespace Generic
{
	enum class FilenameExtReplacementMode : int
	{
		Append,
		Replace
	};

	class SourceFileLoader
	{
	public:
		SourceFileLoader(): m_filename_ext_replacement_mode{FilenameExtReplacementMode::Append}
		{
		}

		explicit SourceFileLoader(std::string&& target_filename_ext,
		                          FilenameExtReplacementMode filename_ext_replacement_mode):
		   m_target_filename_ext{std::move(target_filename_ext)},
		   m_filename_ext_replacement_mode{filename_ext_replacement_mode}
		{
		}

		std::vector<Maike::Db::Dependency> getDependencies(Maike::Io::Reader) const
		{
			return std::vector<Maike::Db::Dependency>{};
		}

		void filterInput(Maike::Io::Reader,
		                 Maike::SourceFileInfoLoaders::SourceOutStream,
		                 Maike::SourceFileInfoLoaders::TagsOutStream) const;

		static char const* name()
		{
			return "generic";
		}

		static Maike::Db::Compiler defaultCompiler()
		{
			return Maike::Db::Compiler{};
		}

		std::string const& targetFilenameExt() const
		{
			return m_target_filename_ext;
		}

		FilenameExtReplacementMode filenameExtReplacementMode() const
		{
			return m_filename_ext_replacement_mode;
		}

	private:
		std::string m_target_filename_ext;
		FilenameExtReplacementMode m_filename_ext_replacement_mode;
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

	FilenameExtReplacementMode fromString(Maike::KeyValueStore::Empty<FilenameExtReplacementMode>,
	                                      char const* str);

	char const* toString(FilenameExtReplacementMode mode);

	inline FilenameExtReplacementMode fromJson(Maike::KeyValueStore::Empty<FilenameExtReplacementMode>,
	                                           Maike::KeyValueStore::JsonRefConst ref)
	{
		return fromString(Maike::KeyValueStore::Empty<FilenameExtReplacementMode>{},
		                  ref.as<char const*>());
	}

	inline SourceFileLoader fromJson(Maike::KeyValueStore::Empty<SourceFileLoader>,
	                                 Maike::KeyValueStore::JsonRefConst ref)
	{
		auto obj = ref.as<Maike::KeyValueStore::CompoundRefConst>();
		auto target_filename_ext = obj.get<char const*>("target_filename_ext");
		auto filename_ext_replacement_mode =
		   obj.getIf<FilenameExtReplacementMode>("filename_ext_replacement_mode");

		return SourceFileLoader{target_filename_ext,
		                        filename_ext_replacement_mode ? *filename_ext_replacement_mode :
		                                                        FilenameExtReplacementMode::Append};
	}

	inline Maike::KeyValueStore::JsonHandle toJson(SourceFileLoader const& obj)
	{
		return Maike::KeyValueStore::Compound{}
		   .set("target_filename_ext", obj.targetFilenameExt())
		   .set("filename_ext_replacement_mode", toString(obj.filenameExtReplacementMode()))
		   .takeHandle();
	}

	Maike::fs::path makeTargetName(Maike::fs::path const& src_path,
	                               std::string const& target_filename_ext,
	                               FilenameExtReplacementMode mode);
}

#endif
