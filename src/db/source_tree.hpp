//@	{
//@	 "targets":[{"name":"source_tree.hpp","type":"include"}]
//@	}

#ifndef MAIKE_DB_SOURCETREE_HPP
#define MAIKE_DB_SOURCETREE_HPP

#include "./source_file_list.hpp"
#include "./dependency_graph.hpp"

namespace Maike::Db
{
	class SourceTree
	{
	public:
		explicit SourceTree(TargetList&& targets, DependencyGraph&& graph):
		   m_targets{std::move(targets)}, m_graph{std::move(graph)}
		{
		}

		TargetList const& targets() const
		{
			return m_targets;
		}

		DependencyGraph const& dependencyGraph() const
		{
			return m_graph;
		}

	private:
		TargetList m_targets;
		DependencyGraph m_graph;
	};
}

#endif