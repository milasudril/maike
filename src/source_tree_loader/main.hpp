//@	 {"targets":[{"name":"main.hpp","type":"include"}]}

#ifndef MAIKE_SOURCETREELOADER_MAIN_HPP
#define MAIKE_SOURCETREELOADER_MAIN_HPP

#include "./directory_scanner.hpp"

namespace Maike::SourceTreeLoader
{
	inline std::map<fs::path, Db::SourceFileInfo> collectSourceFiles(Sched::ThreadPool& workers,
	                                                   fs::path const& src_path,
	                                                   InputFilter const& filter,
	                                                   SourceFileLoaderDelegator const& loaders,
	                                                   fs::path const& target_dir)
	{
		return DirectoryScanner{workers, std::ref(filter), std::ref(loaders)}
		   .processPath(src_path, target_dir)
		   .takeResult();
	}
}

#endif