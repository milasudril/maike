//@	{
//@	  "targets":[{"name":"source_file_record.o","type":"object"}]
//@	}

#include "./source_file_record.hpp"

#include <algorithm>

bool Maike::Db::isUpToDate(SourceFileRecordConst const& rec, std::vector<Dependency> const& deps)
{
	auto const& targets = rec.sourceFileInfo().targets();
	return std::all_of(
	   std::begin(targets), std::end(targets), [&src_file = rec.path(), &deps](auto const& item) {
		   return isUpToDate(item, src_file, deps);
	   });
}