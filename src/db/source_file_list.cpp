//@	{
//@	  "targets":[{"name":"source_file_list.o","type":"object"}]
//@	}

#include "./source_file_list.hpp"
#include "./dependency_graph.hpp"

#include "src/utils/graphutils.hpp"

#include <algorithm>

void Maike::Db::insertSourceFilesForExternalEntries(std::vector<Dependency> const& deps,
                                                    SourceFileList& source_files)
{
	std::for_each(std::begin(deps), std::end(deps), [&source_files](auto const& item) {
		insertSourceFileIfExternal(item, source_files);
	});
}

void Maike::Db::insertDummySourceFiles(SourceFileList::value_type const& item, SourceFileList& list)
{
	auto const& targets = item.second.targets();
	std::for_each(std::begin(targets), std::end(targets), [&list](auto const& target) {
		insertSourceFilesForExternalEntries(target.useDeps(), list);
	});

	auto const& use_deps = item.second.useDeps();
	std::for_each(std::begin(use_deps), std::end(use_deps), [&list](auto const& item) {
		insertSourceFileIfExternal(item, list);
	});
}

Maike::Db::SourceFileList Maike::Db::createDummySourceFiles(SourceFileList const& source_files)
{
	SourceFileList ret;
	std::for_each(std::begin(source_files), std::end(source_files), [&ret](auto const& item) {
		insertDummySourceFiles(item, ret);
	});
	return ret;
}

void Maike::Db::insertUnique(
   std::reference_wrapper<TargetInfo const> target,
   std::reference_wrapper<SourceFileList::value_type const> src_file_entry,
   TargetList& targets)
{
	auto i = targets.find(target.get().name());
	if(i != std::end(targets))
	{
		std::string msg{"Target has "};
		msg += target.get().name();
		msg += " already been defined in ";
		msg += i->second.sourceFilename();
		throw std::runtime_error{std::move(msg)};
	}

	targets.insert(std::make_pair(
	   target.get().name(), Target{src_file_entry.get().first, target, src_file_entry.get().second}));
}

void Maike::Db::collectTargets(
   std::reference_wrapper<SourceFileList::value_type const> src_file_entry, TargetList& output)
{
	auto const& targets = src_file_entry.get().second.targets();
	std::for_each(
	   std::begin(targets), std::end(targets), [&output, &src_file_entry](auto const& target) {
		   // Some targets (include files and directories) share name with source files.
		   // Ignore those.
		   if(src_file_entry.get().first != target.name())
		   { insertUnique(target, src_file_entry, output); }
	   });
}

Maike::Db::TargetList
Maike::Db::collectTargets(std::reference_wrapper<SourceFileList const> source_files)
{
	TargetList ret;
	std::for_each(std::begin(source_files.get()),
	              std::end(source_files.get()),
	              [&ret](auto const& item) { collectTargets(item, ret); });
	return ret;
}

Maike::Db::SourceFileInfo Maike::Db::makeSourceFileInfo(TargetList::value_type const& item,
                                                        DependencyGraph const& g,
                                                        fs::path const& target_dir)
{
	auto use_deps = item.second.useDeps();
	auto node = getNode(g, item.second.sourceFilename());

	// NOTE: At this step there are no source files that could contian recursive linking
	// information.
	//       Thus, this will *only* collect data from all include files.
	Maike::processGraphNodeRecursive(
	   [&use_deps, &target_dir, &target_name = item.first](auto const& node) {
		   auto const& child_target_use_deps = node.sourceFileInfo().childTargetsUseDeps();
		   std::for_each(std::begin(child_target_use_deps),
		                 std::end(child_target_use_deps),
		                 [&use_deps, &target_dir, &target_name](auto const& item) {
			                 if(item.name()
			                    != target_dir / target_name) // A target may never point to itself
			                 {
				                 use_deps.push_back(Maike::Db::Dependency{item.name(), item.expectedOrigin()});
			                 }
		                 });
	   },
	   g,
	   node);

	Maike::Db::SourceFileInfo src_file;
	// FIXME: Remove duplicates before sinking into src_file

	src_file.useDeps(std::move(use_deps))
	   .buildDeps(std::vector{
	      Maike::Db::Dependency{item.second.sourceFilename(), Maike::Db::SourceFileOrigin::Project}});

	return src_file;
}

void Maike::Db::makeSourceFileInfosFromTargets(TargetList const& targets,
                                               SourceFileList& source_files,
                                               fs::path const& target_dir)
{
	Maike::Db::DependencyGraph g{std::move(source_files),
	                             Maike::Db::DependencyGraph::IgnoreResolveErrors{}};
	source_files = g.takeSourceFiles();
	std::for_each(
	   std::begin(targets), std::end(targets), [&g, &source_files, &target_dir](auto const& item) {
		   source_files.insert(
		      std::make_pair(target_dir / item.first, makeSourceFileInfo(item, g, target_dir)));
	   });
}