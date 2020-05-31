//@	{
//@	  "targets":[{"name":"dependency_graph.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"dependency_graph.hpp", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_DEPENDENCYGRAPH_HPP
#define MAIKE_DB_DEPENDENCYGRAPH_HPP

#include "./source_file_record.hpp"
#include "./source_file_index.hpp"

#include <algorithm>
#include <cassert>

namespace Maike::Db
{
	class DependencyGraph
	{
	public:
		explicit DependencyGraph(SourceFileIndex const& index): m_nodes{getRecordsById(index)}
		{
		}

		using node_type = SourceFileRecordConst;

		auto const& nodes() const
		{
			return m_nodes;
		}


	private:
		std::vector<SourceFileRecordConst> m_nodes;
	};

	template<class Function>
	void visitNodes(Function&& f, DependencyGraph const& g)
	{
		auto const& nodes = g.nodes();
		std::for_each(std::begin(nodes), std::end(nodes), std::forward<Function>(f));
	}

	auto size(DependencyGraph const& g)
	{
		return g.nodes().size();
	}

	auto const& getNodeById(DependencyGraph const& g, SourceFileId id)
	{
		auto val = id.value();
		assert(val < size(g));
		return g.nodes()[id.value()];
	}
}

#endif