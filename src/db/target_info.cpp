//@	{
//@	  "targets":[{"name":"target_info.o","type":"object"}]
//@	}

#include "./target_info.hpp"

#include <algorithm>

bool Maike::Db::isUpToDate(TargetInfo const& target,
                           fs::path const& src_file,
                           std::vector<Dependency> const& deps)
{
	return isOlder(src_file, target.name())
	       && std::all_of(
	          std::begin(deps), std::end(deps), [&target_name = target.name()](auto const& item) {
		          if(isExternal(item.expectedOrigin())) { return true; }

		          return isOlder(item.name(), target_name);
	          });
	return true;
}