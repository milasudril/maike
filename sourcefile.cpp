//@	 {"targets":[{"name":"sourcefile.o","type":"object"}]}

#include "./sourcefile.hpp"

Maike::SourceFile::SourceFile(fs::path&& src,
                              std::vector<Dependency>&& used_files,
                              std::vector<fs::path>&& targets,
                              Compiler&& compiler):
   m_name{std::move(src)},
   m_used_files{std::move(used_files)},
   m_targets{std::move(targets)},
   m_compiler{std::move(compiler)}
{
}
