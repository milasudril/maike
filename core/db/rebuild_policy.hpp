//@	{
//@	"dependencies_extra":[{"ref":"./rebuild_policy.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DB_REBUILDPOLICY_HPP
#define MAIKE_DB_REBUILDPOLICY_HPP

#include "core/key_value_store/json_handle.hpp"

namespace Maike::Db
{
	enum class RebuildPolicy : int
	{
		Always,
		OnlyIfOutOfDate
	};

	RebuildPolicy fromString(KeyValueStore::Empty<RebuildPolicy>, char const* str);

	inline auto fromJson(KeyValueStore::Empty<RebuildPolicy>, Maike::KeyValueStore::JsonRefConst val)
	{
		return fromString(KeyValueStore::Empty<RebuildPolicy>{}, val.as<char const*>());
	}

	char const* toString(RebuildPolicy);
}

#endif