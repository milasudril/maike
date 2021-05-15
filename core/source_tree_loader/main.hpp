//@	{
//@	"dependencies_extra":[{"ref":"./main.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SOURCETREELOADER_MAIN_HPP
#define MAIKE_SOURCETREELOADER_MAIN_HPP

#include "./directory_scanner.hpp"

#include "core/db/source_tree.hpp"

namespace Maike::SourceTreeLoader
{
	inline std::map<fs::path, Db::SourceFileInfo>
	collectSourceFiles(Sched::ThreadPool& workers,
	                   fs::path const& src_path,
	                   InputFilter const& filter,
	                   SourceFileLoaderDelegator const& loaders,
	                   RecursiveScan recursive,
	                   fs::path const& target_dir)
	{
		return DirectoryScanner{workers, std::ref(filter), std::ref(loaders), recursive}
		   .processPath(src_path, target_dir)
		   .takeResult();
	}

	Db::SourceTree load(Sched::ThreadPool& workers,
	                    fs::path const& src_path,
	                    InputFilter const& filter,
	                    SourceFileLoaderDelegator const& loaders,
	                    RecursiveScan recursive,
	                    fs::path const& target_dir);
}

#endif
