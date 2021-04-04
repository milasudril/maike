//@	{
//@	  "targets":[{"name":"source_file_record.o","type":"object"}]
//@	 ,"dependenies_extra":[{"ref":"source_file_record.o","rel":"implementation"}]
//@	}

#include "./source_file_record.hpp"

#include <algorithm>


bool Maike::Db::isUpToDate(TargetInfo const& target,
                           fs::path const& src_file,
                           std::vector<Dependency> const& deps)
{
	return older(src_file, target.name())
	       && std::all_of(
	          std::begin(deps), std::end(deps), [&target_name = target.name()](auto const& item) {
		          if(isExternal(item.expectedOrigin())) { return true; }

		          return older(item.name(), target_name);
	          });
	return true;
}

bool Maike::Db::isUpToDate(SourceFileRecordConst const& rec, std::vector<Dependency> const& deps)
{
	auto const& targets = rec.sourceFileInfo().targets();
	return std::all_of(
	   std::begin(targets), std::end(targets), [&src_file = rec.path(), &deps](auto const& item) {
		   return isUpToDate(item, src_file, deps);
	   });
}