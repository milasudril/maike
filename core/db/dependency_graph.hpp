//@	{
//@	 "dependencies_extra":[{"ref":"./dependency_graph.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_DEPENDENCYGRAPH_HPP
#define MAIKE_DB_DEPENDENCYGRAPH_HPP

#include "./source_file_record.hpp"
#include "./invoker.hpp"
#include "./compilation_log.hpp"

#include "core/sched/batch.hpp"
#include "core/build/info.hpp"

#include <algorithm>
#include <map>
#include <cassert>

namespace Maike::Db
{
	class DependencyGraph
	{
	public:
		class IgnoreResolveErrors
		{
		public:
			explicit constexpr IgnoreResolveErrors(bool value = true): m_value{value}
			{
			}

			constexpr operator bool() const
			{
				return m_value;
			}

		private:
			bool m_value;
		};

		explicit DependencyGraph(std::map<fs::path, SourceFileInfo>&& src_files,
		                         IgnoreResolveErrors resolve_errors = IgnoreResolveErrors{false});

		using node_type = SourceFileRecordConst;

		using edge_type = Dependency;

		auto const& nodes() const
		{
			return m_nodes;
		}

		auto&& takeSourceFiles()
		{
			return std::move(m_src_files);
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

	SourceFileRecordConst getNode(DependencyGraph const& g, Maike::fs::path const& name);

	std::vector<Db::Dependency> getUseDepsRecursive(DependencyGraph const& g,
	                                                SourceFileRecordConst const& rec);

	class ForceRecompilation
	{
	public:
		explicit ForceRecompilation(bool value = true): m_value{value}
		{
		}

		constexpr operator bool() const
		{
			return m_value;
		}

	private:
		bool m_value;
	};

	void compile(DependencyGraph const& g,
	             SourceFileRecordConst const& node,
	             Build::Info const& build_info,
	             Invoker invoker,
	             CompilationLog& log,
	             ForceRecompilation force_recompilation,
	             Sched::Batch const& ctxt);
}

#endif
