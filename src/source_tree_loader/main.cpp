//@	 {"targets":[{"name":"main.o","type":"object"}]}

#include "./main.hpp"

#include "src/db/source_file_list.hpp"

Maike::Db::SourceTree Maike::SourceTreeLoader::load(Sched::ThreadPool& workers,
                                                    fs::path const& src_path,
                                                    InputFilter const& filter,
                                                    SourceFileLoaderDelegator const& loaders,
                                                    fs::path const& target_dir)
{
	auto src_files = collectSourceFiles(workers, src_path, filter, loaders, target_dir);
	auto targets = collectTargets(src_files);
	makeSourceFileInfos(targets, src_files);
	src_files.merge(createPlaceholdersForExternalEntries(src_files));
	return Db::SourceTree{
	   std::move(targets), Db::DependencyGraph{std::move(src_files)}, fs::path{target_dir}};
}