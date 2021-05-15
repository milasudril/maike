//@	{
//@	  "targets":[{"name":"source_file_origin.o","type":"object"}]
//@	 }

#include "./source_file_origin.hpp"

#include <cstring>

Maike::Db::SourceFileOrigin Maike::Db::fromString(KeyValueStore::Empty<SourceFileOrigin>,
                                                  char const* val)
{
	if(strcmp(val, "generated") == 0) { return SourceFileOrigin::Generated; }
	else if(strcmp(val, "project") == 0)
	{
		return SourceFileOrigin::Project;
	}
	else if(strcmp(val, "system") == 0)
	{
		return SourceFileOrigin::System;
	}
	else if(strcmp(val, "pkg-config") == 0)
	{
		return SourceFileOrigin::PkgConfig;
	}

	throw std::runtime_error{"Unknown origin"};
}

char const* Maike::Db::toString(SourceFileOrigin val)
{
	switch(val)
	{
		case SourceFileOrigin::Project: return "project";

		case SourceFileOrigin::System: return "system";

		case SourceFileOrigin::PkgConfig: return "pkg-config";

		case SourceFileOrigin::Generated: return "generated";
	}
	__builtin_unreachable();
}