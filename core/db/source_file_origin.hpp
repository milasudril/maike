//@	{
//@	 "dependencies_extra":[{"ref":"./source_file_origin.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_SOURCEFILEORIGIN_HPP
#define MAIKE_DB_SOURCEFILEORIGIN_HPP

#include "core/key_value_store/json_handle.hpp"

namespace Maike::Db
{
	enum class SourceFileOrigin : int
	{
		Project,
		Generated,
		System,
		PkgConfig
	};

	constexpr bool isExternal(SourceFileOrigin origin)
	{
		return origin == SourceFileOrigin::System || origin == SourceFileOrigin::PkgConfig;
	}

	SourceFileOrigin fromString(KeyValueStore::Empty<SourceFileOrigin>, char const* str);

	char const* toString(SourceFileOrigin);
}

#endif
