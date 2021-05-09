//@	{
//@	"dependencies_extra":[{"ref":"./rebuild_policy.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DB_REBUILDPOLICY_HPP
#define MAIKE_DB_REBUILDPOLICY_HPP

#include "src/key_value_store/json_handle.hpp"

namespace Maike::Db
{
	enum class RebuildPolicy : int
	{
		Always,
		OnlyIfOutOfDate
	};

	RebuildPolicy fromString(KeyValueStore::Empty<RebuildPolicy>, char const* str);

	char const* toString(RebuildPolicy);
}

#endif