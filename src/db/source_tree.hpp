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

	class TaskCounter
	{
	public:
		constexpr explicit TaskCounter(size_t n): m_value{n}
		{
		}

		constexpr size_t value() const
		{
			return m_value;
		}

		TaskCounter& operator+=(TaskCounter other)
		{
			m_value += other.value();
			return *this;
		}

	private:
		size_t m_value;
	};

	TaskCounter compile(SourceTree const& src_tree, ForceRecompilation, Sched::ThreadPool& workers);

	TaskCounter compile(SourceTree const& src_tree,
	                    ForceRecompilation force_recompilation,
	                    Sched::ThreadPool& workers,
	                    std::pair<fs::path const*, size_t> targets);

	TaskCounter compile(SourceTree const& src_tree,
	                    ForceRecompilation force_recompilation,
	                    Sched::ThreadPool& workers,
	                    fs::path const& target_name);
}

#endif