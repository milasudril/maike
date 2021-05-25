//@	{
//@	 "dependencies_extra":[{"ref":"./source_file_list.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_SOURCEFILELIST_HPP
#define MAIKE_DB_SOURCEFILELIST_HPP

#include "./source_file_info.hpp"
#include "./target.hpp"
#include "./dependency_graph.hpp"

#include <map>

namespace Maike::Db
{
	using SourceFileList = DependencyGraph::SourceFileList;
	using TargetList = std::map<Maike::fs::path, Maike::Db::Target>;

	inline void insertPlaceholderIfExternal(Dependency const& dep, SourceFileList& source_files)
	{
		if(isExternal(dep))
		{
			source_files.insert(
			   std::make_pair(dep.name(), SourceFileInfo{dep.expectedOrigin(), Compiler{"", false}}));
		}
	}

	void insertPlaceholdersForExternalEntries(std::vector<Dependency> const& deps,
	                                          SourceFileList& source_files);

	void insertPlaceholdersForExternalEntries(SourceFileList::value_type const& item,
	                                          SourceFileList& list);

	SourceFileList createPlaceholdersForExternalEntries(SourceFileList const& source_files);

	void insertUnique(std::reference_wrapper<TargetInfo const> target,
	                  std::reference_wrapper<SourceFileList::value_type const> src_file_entry,
	                  TargetList& targets);

	void collectTargets(std::reference_wrapper<SourceFileList::value_type const> src_file_entry,
	                    TargetList& output);

	TargetList collectTargets(std::reference_wrapper<SourceFileList const> source_files);

	SourceFileInfo makeSourceFileInfo(TargetList::value_type const& item, DependencyGraph const& g);

	void makeSourceFileInfos(TargetList const& targets, SourceFileList& source_files);

	void addTargetDepsToSourceFiles(SourceFileList& source_files);

}

#endif
