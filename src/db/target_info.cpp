//@	{
//@	  "targets":[{"name":"target_info.o","type":"object"}]
//@	}

#include "./target_info.hpp"

#include <algorithm>

#include <mutex>

bool Maike::Db::isUpToDate(TargetInfo const& target,
                           fs::path const& src_file,
                           std::vector<Dependency> const& deps)
{
	std::error_code ec{};
	if(is_directory(target.name(), ec)) { return true; }

	return detail::call_is_older(src_file, target.name())
	       && std::all_of(
	          std::begin(deps), std::end(deps), [&target_name = target.name()](auto const& item) {
		          return isOlder(item, target_name);
	          });
}

static std::mutex m;

void Maike::Db::compile(Compiler const& compiler,
                        std::vector<TargetInfo> const& targets,
                        fs::path const& src_file,
                        std::vector<Dependency> const&)
{
	if(std::size(targets) == 0) { return; }

	std::lock_guard lock{m};
	printf("%s -std=c++17 -o '%s' '%s'",
	       compiler.recipe().c_str(),
	       targets[0].name().c_str(),
	       src_file.c_str());
	//	std::for_each(std::rbegin(deps), std::rend(deps), [](auto const& item) {
	//		printf(" '%s'", item.name().c_str());
	//	});
	printf("\n");
}