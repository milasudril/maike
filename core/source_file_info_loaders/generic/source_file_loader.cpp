//@	{"target":{"name":"source_file_loader.o"}}

#include "./source_file_loader.hpp"

#include <stdexcept>
#include <string>
#include <cstring>

Generic::FilenameExtReplacementMode
Generic::fromString(Maike::KeyValueStore::Empty<FilenameExtReplacementMode>, char const* str)
{
	if(strcmp(str, "append") == 0) { return FilenameExtReplacementMode::Append; }

	if(strcmp(str, "replace_last") == 0) { return FilenameExtReplacementMode::ReplaceLast; }

	if(strcmp(str, "replace_full") == 0) { return FilenameExtReplacementMode::ReplaceFull; }

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
		case FilenameExtReplacementMode::ReplaceLast: return "replace_last";
		case FilenameExtReplacementMode::ReplaceFull: return "replace_full";
	}
	__builtin_unreachable();
}