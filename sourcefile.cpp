//@	 {"targets":[{"name":"sourcefile.o","type":"object"}]}

#include "./sourcefile.hpp"

Maike::SourceFile::SourceFile(fs::path&& src,
                              std::vector<Dependency>&& used_files,
                              std::vector<fs::path>&& output_files,
                              Compiler&& compiler):
m_name{std::move(src)},
m_used_files{std::move(used_files)},
m_output_files{std::move(output_files)},
m_compiler{std::move(compiler)}
{}

