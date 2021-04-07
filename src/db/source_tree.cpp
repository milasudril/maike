//@	{
//@	 "targets":[{"name":"source_tree.o","type":"object"}]
//@	}

#include "./source_tree.hpp"

#include "src/utils/graphutils.hpp"
#include "src/sched/signaling_counter.hpp"

#include <thread>

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

void Maike::Db::compile(SourceTree const& src_tree,
                        ForceRecompilation force_recompilation,
                        Sched::ThreadPool& workers)
{
	visitNodesInTopoOrder(
	   [&graph = src_tree.dependencyGraph(),
	    force_recompilation,
	    ctxt =
	       CompilationContext{size(src_tree.dependencyGraph()), workers}](auto const& node) mutable {
		   ctxt.process(node.id(), [&graph, &node, force_recompilation, &ctxt]() {
			   compile(graph, node, force_recompilation, ctxt);
			   std::this_thread::sleep_for(std::chrono::seconds{2});
		   });
	   },
	   src_tree.dependencyGraph());
}

void Maike::Db::compile(SourceTree const& src_tree,
                        ForceRecompilation force_recompilation,
						Sched::ThreadPool& workers,
                        fs::path const& target_name)
{
	auto const& target = getTarget(src_tree, target_name);
	auto const& graph = src_tree.dependencyGraph();
	auto const& node = getNode(graph, target.sourceFilename());

	processGraphNodeRecursive(
	   [&graph = src_tree.dependencyGraph(),
	    force_recompilation,
	    ctxt =
	       CompilationContext{size(src_tree.dependencyGraph()), workers}](auto const& node) mutable {
		   ctxt.process(node.id(), [&graph, &node, force_recompilation, &ctxt]() {
			   compile(graph, node, force_recompilation, ctxt);
			   std::this_thread::sleep_for(std::chrono::seconds{2});
		   });
	   },
	   graph,
	   node);
}