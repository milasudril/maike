//@	{
//@	  "targets":[{"name":"source_file_record.o","type":"object"}]
//@	 ,"dependenies_extra":[{"ref":"source_file_record.o","rel":"implementation"}]
//@	}

#include "./source_file_record.hpp"

#include <algorithm>

bool Maike::Db::isUpToDate(SourceFileRecordConst const& rec, std::vector<Dependency> const& deps)
{
	auto const& targets = rec.sourceFileInfo().targets();
	if(std::any_of(
	      std::begin(targets), std::end(targets), [name = rec.path(), &deps](auto const& item) {
		      auto const target_name = item.name();
		      auto const a = newer(name, target_name);
		      auto const b =
		         std::any_of(std::begin(deps), std::end(deps), [target_name](auto const& item) {
			         return newer(item.name(), target_name);
		         });

		      return a || b;
	      }))
	{ return false; }
	return true;
}