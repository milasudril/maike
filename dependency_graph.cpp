//@	{
//@	  "targets":[{"name":"dependency_graph.o","type":"object"}]
//@	}

#include "./dependency_graph.hpp"

#include <algorithm>

std::pair<Maike::fs::path const, Maike::SourceFileInfo> const&
Maike::DependencyGraph::insert(fs::path&& src_file_name, SourceFileInfo&& src_file_info)
{
	auto const& targets = src_file_info.targets();
	std::for_each(std::begin(targets), std::end(targets), [this](auto const& item) {
		m_sources.insert(std::make_pair(item, SourceFileInfo{}));
	});

	return *m_sources.insert(std::make_pair(std::move(src_file_name), std::move(src_file_info))).first;
}


Maike::SourceFileInfo const* Maike::DependencyGraph::find(fs::path const& src_file) const
{
	auto i = m_sources.find(src_file);
	return i == m_sources.end() ? nullptr : &(i->second);
}
