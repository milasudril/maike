//@	{
//@	  "targets":[{"name":"dependency_graph.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"dependency_graph.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_DEPENDENCYGRAPH_HPP
#define MAIKE_DB_DEPENDENCYGRAPH_HPP

#include "./source_file_record.hpp"

#include <algorithm>
#include <map>
#include <cassert>

namespace Maike::Db
{
	class DependencyGraph
	{
	public:
		explicit DependencyGraph(std::map<fs::path, SourceFileInfo>&& src_files);

		using node_type = SourceFileRecordConst;

		auto const& nodes() const
		{
			return m_nodes;
		}


	private:
		std::map<fs::path, SourceFileInfo> m_src_files;
		std::vector<SourceFileRecordConst> m_nodes;
	};

	template<class Function>
	void visitNodes(Function&& f, DependencyGraph const& g)
	{
		auto const& nodes = g.nodes();
		std::for_each(std::begin(nodes), std::end(nodes), std::forward<Function>(f));
	}

	inline auto size(DependencyGraph const& g)
	{
		return g.nodes().size();
	}

	inline auto const& getNodeById(DependencyGraph const& g, SourceFileId id)
	{
		auto val = id.value();
		assert(val < size(g));
		return g.nodes()[id.value()];
	}
}

#endif