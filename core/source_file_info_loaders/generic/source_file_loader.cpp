//@	{"target":{"name":"source_file_loader.o"}}

#include "./source_file_loader.hpp"

#include <stdexcept>
#include <string>
#include <cstring>

Generic::FilenameExtReplacementMode
Generic::fromString(Maike::KeyValueStore::Empty<FilenameExtReplacementMode>, char const* str)
{
	if(strcmp(str, "append") == 0) { return FilenameExtReplacementMode::Append; }

	if(strcmp(str, "replace") == 0) { return FilenameExtReplacementMode::Replace; }

	std::string msg{"Bad filename extension replacement mode `"};
	msg += str;
	msg += "`";

	throw std::runtime_error{std::move(msg)};
}

char const* Generic::toString(FilenameExtReplacementMode mode)
{
	switch(mode)
	{
		case FilenameExtReplacementMode::Append: return "append";
		case FilenameExtReplacementMode::Replace: return "replace";
	}
	__builtin_unreachable();
}

Maike::fs::path Generic::makeTargetName(Maike::fs::path const& src_path,
                                        std::string const& target_filename_ext,
                                        FilenameExtReplacementMode mode)
{
	switch(mode)
	{
		case FilenameExtReplacementMode::Append: return src_path.string() + target_filename_ext;
		case FilenameExtReplacementMode::Replace:
			return Maike::fs::path{src_path}.replace_extension(target_filename_ext);
	}
	__builtin_unreachable();
}

void Generic::SourceFileLoader::filterInput(
   Maike::Io::Reader src,
   Maike::SourceFileInfoLoaders::SourceOutStream,
   Maike::SourceFileInfoLoaders::TagsOutStream tag_out) const
{
	Maike::KeyValueStore::Compound tag;
	tag.set("target",
	        Maike::KeyValueStore::Compound{}.set("name",
	                                             makeTargetName(src.sourcePath().filename(),
	                                                            m_target_filename_ext,
	                                                            m_filename_ext_replacement_mode)));

	store(tag, tag_out, Maike::KeyValueStore::PrettyPrint{false});
}
