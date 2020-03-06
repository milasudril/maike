//@	 {"targets":[{"name":"source_file_info.o","type":"object"}]}

#include "./source_file_info.hpp"

#include <algorithm>

Maike::SourceFileInfo::SourceFileInfo(std::vector<Dependency>&& used_files,
                                      fs::path const& target_dir,
                                      std::vector<fs::path>&& targets,
                                      Compiler&& compiler):
   m_used_files{std::move(used_files)},
   m_targets{std::move(targets)},
   m_compiler{std::move(compiler)}
{
	std::for_each(std::begin(m_targets), std::end(m_targets), [&target_dir](auto& item) {
		item = target_dir / item;
	});
}