//@	{
//@	  "targets":[{"name":"dependency_graph.o","type":"object"}]
//@	}

#include "./dependency_graph.hpp"

#include <algorithm>

Maike::SourceFile<false>
Maike::DependencyGraph::insert(fs::path&& src_file_name, SourceFileInfo&& src_file_info)
{
	auto const& targets = src_file_info.targets();
	std::for_each(std::begin(targets), std::end(targets), [this](auto const& item) {
		m_sources.insert(std::make_pair(item, SourceFileInfo{}));
	});

	auto i = m_sources.insert(std::make_pair(std::move(src_file_name), std::move(src_file_info)));

	return SourceFile<false>{i.first->first, i.first->second};
}


Maike::SourceFile<true> Maike::DependencyGraph::find(fs::path const& src_file) const
{
	auto i = m_sources.find(src_file);
	return i == m_sources.end() ? SourceFile<true>{} : SourceFile<true>{i->first, i->second};
}

Maike::SourceFile<false> Maike::DependencyGraph::find(fs::path const& src_file)
{
	auto i = m_sources.find(src_file);
	return i == m_sources.end() ? SourceFile<false>{} : SourceFile<false>{i->first, i->second};
}
