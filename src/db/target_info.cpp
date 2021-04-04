//@	{
//@	  "targets":[{"name":"target_info.o","type":"object"}]
//@	}

#include "./target_info.hpp"

#include <algorithm>

bool Maike::Db::isUpToDate(TargetInfo const& target,
                           fs::path const& src_file,
                           std::vector<Dependency> const& deps)
{
	return detail::call_is_older(src_file, target.name())
	       && std::all_of(
	          std::begin(deps), std::end(deps), [&target_name = target.name()](auto const& item) {
		          return isOlder(item, target_name);
	          });
	return true;
}