//@	{
//@	 "targets":[{"name":"source_tree.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_tree.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DB_SOURCETREE_HPP
#define MAIKE_DB_SOURCETREE_HPP

#include "./source_file_list.hpp"
#include "./dependency_graph.hpp"

#include "src/sched/thread_pool.hpp"

namespace Maike::Db
{
	class SourceTree
	{
	public:
		explicit SourceTree(TargetList&& targets, DependencyGraph&& graph, fs::path&& target_dir):
		   m_targets{std::move(targets)}, m_graph{std::move(graph)}, m_target_dir{std::move(target_dir)}
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

		fs::path const& targetDirectory() const
		{
			return m_target_dir;
		}

	private:
		TargetList m_targets;
		DependencyGraph m_graph;
		fs::path m_target_dir;
	};

	Target const& getTarget(SourceTree const& src_tree, fs::path const& target_name);

	void compile(SourceTree const& src_tree, Sched::ThreadPool& workers);
	void compile(SourceTree const& src_tree, fs::path const& target_name, Sched::ThreadPool& workers);
	void compileAlways(SourceTree const& src_tree, Sched::ThreadPool& workers);
	void compileAlways(SourceTree const& src_tree,
	                   fs::path const& target_name,
	                   Sched::ThreadPool& workers);
}

#endif