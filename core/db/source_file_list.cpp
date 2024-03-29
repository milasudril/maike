//@	{
//@	  "target":{"name":"source_file_list.o"}
//@	}

#include "./source_file_list.hpp"
#include "./dependency_graph.hpp"

#include "core/utils/graphutils.hpp"

#include <algorithm>

void Maike::Db::insertPlaceholdersForExternalEntries(std::vector<Dependency> const& deps,
                                                     SourceFileList& source_files)
{
	std::for_each(std::begin(deps), std::end(deps), [&source_files](auto const& item) {
		insertPlaceholderIfExternal(item, source_files);
	});
}

void Maike::Db::insertPlaceholdersForExternalEntries(SourceFileList::value_type const& item,
                                                     SourceFileList& list)
{
	auto const& targets = item.second.targets();
	std::for_each(std::begin(targets), std::end(targets), [&list](auto const& target) {
		insertPlaceholdersForExternalEntries(target.useDeps(), list);
	});

	auto const& use_deps = item.second.useDeps();
	std::for_each(std::begin(use_deps), std::end(use_deps), [&list](auto const& item) {
		insertPlaceholderIfExternal(item, list);
	});
}

Maike::Db::SourceFileList
Maike::Db::createPlaceholdersForExternalEntries(SourceFileList const& source_files)
{
	SourceFileList ret;
	std::for_each(std::begin(source_files), std::end(source_files), [&ret](auto const& item) {
		insertPlaceholdersForExternalEntries(item, ret);
	});
	return ret;
}

void Maike::Db::insertUnique(
   std::reference_wrapper<TargetInfo const> target,
   std::reference_wrapper<SourceFileList::value_type const> src_file_entry,
   TargetList& targets)
{
	auto name = target.get().name().lexically_normal();
	auto i = targets.find(name);
	if(i != std::end(targets))
	{
		std::string msg{"Target has "};
		msg += name;
		msg += " has already been defined in ";
		msg += i->second.sourceFilename();
		throw std::runtime_error{std::move(msg)};
	}

	targets.insert(std::make_pair(
	   std::move(name), Target{src_file_entry.get().first, target, src_file_entry.get().second}));
}

void Maike::Db::collectTargets(
   std::reference_wrapper<SourceFileList::value_type const> src_file_entry, TargetList& output)
{
	auto const& targets = src_file_entry.get().second.targets();
	std::for_each(
	   std::begin(targets), std::end(targets), [&output, &src_file_entry](auto const& target) {
		   // Some targets (include-files and directories) may share name with source files.
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
                                                        DependencyGraph const& g)
{
	auto use_deps = item.second.useDeps();
	auto node = getNode(g, item.second.sourceFilename());

	// NOTE: At this step there are no source files that could contian recursive linking
	// information. Thus, this will *only* collect data from all include files.
	processGraphNodeRecursive(
	   [&use_deps, &target_name = item.first](SourceFileRecordConst const& node, auto const) {
		   auto const& child_target_use_deps = node.sourceFileInfo().childTargetsUseDeps();
		   std::for_each(std::begin(child_target_use_deps),
		                 std::end(child_target_use_deps),
		                 [&use_deps, &target_name](auto const& item) {
			                 // Since an implementation file may include the interface file, it may
			                 // happen that a target has itself as a use dependency. To prevent this
			                 // situation, skip adding this dependency.
			                 if(item.name() != target_name) { use_deps.push_back(item); }
		                 });
	   },
	   g,
	   node);

	SourceFileInfo src_file{SourceFileOrigin::Generated, Compiler{"", false}};

	src_file.useDeps(std::move(use_deps))
	   .buildDeps(std::vector{Dependency{
	      item.second.sourceFilename(), SourceFileOrigin::Project, std::vector<Property>{}}});

	return src_file;
}

void Maike::Db::makeSourceFileInfos(TargetList const& targets, SourceFileList& source_files)
{
	DependencyGraph g{SourceFileList{source_files}, Db::DependencyGraph::IgnoreResolveErrors{}};
	std::for_each(std::begin(targets), std::end(targets), [&g, &source_files](auto const& item) {
		source_files.insert(std::make_pair(item.first, makeSourceFileInfo(item, g)));
	});
}

void Maike::Db::addTargetDepsToSourceFiles(SourceFileList& source_files)
{
	std::for_each(std::begin(source_files), std::end(source_files), [&source_files](auto& item) {
		if(item.second.useTargetDeps())
		{
			auto deps = item.second.useDeps();
			auto& targets = item.second.targets();
			std::for_each(std::begin(targets), std::end(targets), [&deps, &source_files](auto const& item) {
				auto i = source_files.find(item.name());
				if(i == std::end(source_files)) { return; }

				auto const& use_deps_target = i->second.useDeps();
				std::copy(std::begin(use_deps_target), std::end(use_deps_target), std::back_inserter(deps));
			});
			item.second.useDeps(std::move(deps));
		}
	});
}
