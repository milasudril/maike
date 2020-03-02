//@	 {"targets":[{"name":"source_file.o","type":"object"}]}

#include "./source_file.hpp"

#include <algorithm>

Maike::SourceFile::SourceFile(fs::path&& src,
                              std::vector<Dependency>&& used_files,
                              fs::path const& target_dir,
                              std::vector<fs::path>&& targets,
                              Compiler&& compiler):
   m_name{std::move(src)},
   m_used_files{std::move(used_files)},
   m_targets{std::move(targets)},
   m_compiler{std::move(compiler)}
{
	std::for_each(std::begin(m_targets), std::end(m_targets), [&target_dir](auto& item) {
		item = target_dir / item;
	});
}

Maike::SourceFile::SourceFile(fs::path const& src): m_name{src}
{
}