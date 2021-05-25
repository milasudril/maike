//@	{
//@	 "targets":[{"name":"source_tree.o","type":"object"}]
//@	}

#include "./source_tree.hpp"
#include "./task_list.hpp"

#include "core/utils/graphutils.hpp"
#include "core/sched/signaling_counter.hpp"

#include <list>
#include <thread>
#include <atomic>

Maike::Db::Target const& Maike::Db::getTarget(SourceTree const& src_tree,
                                              fs::path const& target_name)
{
	auto const& targets = src_tree.targets();
	auto i = targets.find(src_tree.targetDirectory() / target_name);
	if(i == std::end(targets))
	{
		std::string msg{"Unkonwn target `"};
		msg += target_name;
		msg += "`";
		throw std::runtime_error{std::move(msg)};
	}

	return i->second;
}

Maike::Db::TaskCounter Maike::Db::compile(SourceTree const& src_tree,
                                          Build::Info const& build_info,
                                          Invoker invoker,
                                          CompilationLog& compilation_log,
                                          Task::ForceRecompilation force_recompilation,
                                          Sched::ThreadPool& workers)
{
	TaskList tasks{src_tree.dependencyGraph(), build_info, compilation_log};
	tasks.process(workers, invoker, force_recompilation);
	return TaskCounter{std::size(tasks)};
}

#if 0
Maike::Db::TaskCounter Maike::Db::compile(SourceTree const& src_tree,
                                          Build::Info const& build_info,
                                          Invoker invoker,
                                          CompilationLog& compilation_log,
                                          ForceRecompilation force_recompilation,
                                          Sched::ThreadPool& workers,
                                          fs::path const& target_name)
{
	auto const& target = getTarget(src_tree, target_name);
	auto const& graph = src_tree.dependencyGraph();
	auto const& node = getNode(graph, target.sourceFilename());

	Sched::Batch ctxt{size(src_tree.dependencyGraph()), workers};

	processGraphNodeRecursive(
	   [&graph = src_tree.dependencyGraph(),
	    &build_info,
	    invoker,
	    &compilation_log,
	    force_recompilation,
	    &ctxt](SourceFileRecordConst const& node, auto const&...) {
		   ctxt.add(
		      node.id().value(),
		      [&graph, &node, &build_info, invoker, &compilation_log, force_recompilation, &ctxt]() {
			      compile(graph, node, build_info, invoker, compilation_log, force_recompilation, ctxt);
		      });
	   },
	   graph,
	   node);

	return TaskCounter{ctxt.throwAnyPendingException().taskCount()};
}

Maike::Db::TaskCounter Maike::Db::compile(SourceTree const& src_tree,
                                          Build::Info const& build_info,
                                          Invoker invoker,
                                          CompilationLog& compilation_log,
                                          ForceRecompilation force_recompilation,
                                          Sched::ThreadPool& workers,
                                          std::pair<fs::path const*, size_t> targets)
{
	TaskCounter n{0};
	std::for_each_n(
	   targets.first,
	   targets.second,
	   [&n, &src_tree, &build_info, invoker, &compilation_log, force_recompilation, &workers](
	      auto const& target_name) {
		   n += compile(
		      src_tree, build_info, invoker, compilation_log, force_recompilation, workers, target_name);
	   });

	return n;
}
#endif