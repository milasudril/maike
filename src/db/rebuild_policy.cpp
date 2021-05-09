//@	{
//@	  "targets":[{"name":"rebuild_policy.o","type":"object"}]
//@	 }

#include "./rebuild_policy.hpp"

#include <cstring>

Maike::Db::RebuildPolicy Maike::Db::fromString(KeyValueStore::Empty<RebuildPolicy>, char const* val)
{
	if(strcmp(val, "always") == 0) { return RebuildPolicy::Always; }
	else if(strcmp(val, "only_if_out_of_date") == 0)
	{
		return RebuildPolicy::OnlyIfOutOfDate;
	}

	throw std::runtime_error{"Unknown origin"};
}

char const* Maike::Db::toString(RebuildPolicy val)
{
	switch(val)
	{
		case RebuildPolicy::Always: return "always";

		case RebuildPolicy::OnlyIfOutOfDate: return "only_if_out_of_date";
	}
	__builtin_unreachable();
}