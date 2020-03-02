//@	{
//@	  "targets":[{"name":"dependency_graph.o","type":"object"}]
//@	}

#include "./dependency_graph.hpp"

#include <algorithm>

Maike::SourceFile const& Maike::DependencyGraph::insert(SourceFile&& src_file)
{
// TODO:
//	auto const& targets = src_file.targets();
//	std::for_each(std::begin(targets), std::end(targets), [this](auto const& item) {
//		m_sources.insert(item);
//	});

	return *m_sources.insert(std::move(src_file)).first;
}


Maike::SourceFile const* Maike::DependencyGraph::find(SourceFile const& src_file) const
{
	auto i = m_sources.find(src_file);
	return i == m_sources.end()? nullptr : &(*i);
}

Maike::SourceFile const* Maike::DependencyGraph::find(fs::path const& src_file) const
{
	auto i = m_sources.find(src_file);
	return i == m_sources.end()? nullptr : &(*i);
}
