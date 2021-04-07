//@	{
//@	 "targets":[{"name":"source_tree.o","type":"object"}]
//@	}

#include "./source_tree.hpp"

#include "src/utils/graphutils.hpp"

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

#if 0
void doIt()
{
	size_t n = 0;
	Sched::SignalingCounter<size_t> c{0};
	processNodes(graph,
	             [&n, &c, results = std::make_unique<Sched::TaskCompletionEvent[]>(size(graph))](
	                auto const& node) mutable {
		             workers.addTask([&graph, &c, &results, &node]() {
			   OnScopeExit task_result{[&e = results[static_cast<size_t>(node.id()]](){
				e.taskFailed();
						}};
			   OnScopeExit task_completion{[&c]() { ++c; }};

			   auto use_deps = getUseDepsRecursive(graph, node);
			   if(std::any_of(std::begin(use_deps), std::end(use_deps), [&ctxt](auto const& item) {
				      return ctxt.waitForTarget(item.reference()) == Sched::TaskResult::Failure;
			      }))
			   { throw std::runtime_error{"Build failed"}; }
			   if(!isUpToDate(node, use_deps)) { compile(node, use_deps); }
			   task_result.get().taskSuceceded();
		             });
		             ++n;
	             });
	c.wait(n);
}
#endif

void Maike::Db::compile(SourceTree const& src_tree,
                        ForceRecompilation force_recompilation,
                        Sched::ThreadPool& workers)
{
	visitNodesInTopoOrder(
	   [&graph = src_tree.dependencyGraph(),
	    force_recompilation,
	    ctxt = makeCompilationContext(src_tree.dependencyGraph(), workers)](auto const& node) mutable {
		   compile(graph, node, force_recompilation, ctxt);
	   },
	   src_tree.dependencyGraph());
}

void Maike::Db::compile(SourceTree const& src_tree,
                        ForceRecompilation force_recompilation,
                        fs::path const& target_name,
                        Sched::ThreadPool& workers)
{
	auto const& target = getTarget(src_tree, target_name);
	auto const& graph = src_tree.dependencyGraph();
	auto const& node = getNode(graph, target.sourceFilename());

	processGraphNodeRecursive([&graph = src_tree.dependencyGraph(),
	                           force_recompilation,
	                           ctxt = makeCompilationContext(src_tree.dependencyGraph(), workers)](
	                             auto const& node) mutable { compile(graph, node, force_recompilation, ctxt); },
	                          graph,
	                          node);
}