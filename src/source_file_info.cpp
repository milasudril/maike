//@	 {"targets":[{"name":"source_file_info.o","type":"object"}]}

#include "./source_file_info.hpp"

Maike::SourceFileInfo::SourceFileInfo(std::vector<Dependency>&& used_files,
                                      std::vector<fs::path>&& targets,
                                      Compiler&& compiler):
   m_used_files{std::move(used_files)},
   m_targets{std::move(targets)},
   m_compiler{std::move(compiler)}
{
}