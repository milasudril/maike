//@	 {"targets":[{"name":"sourcefile.o","type":"object"}]}

#include "./sourcefile.hpp"

Maike::SourceFile::SourceFile(Path&& src,
                              std::vector<Path const*>&& used_files,
                              std::vector<Path const*>&& output_files,
                              Compiler&& compiler):
m_name{std::move(src)},
m_used_files{std::move(used_files)},
m_output_files{std::move(output_files)},
m_compiler{std::move(compiler)}
{}

