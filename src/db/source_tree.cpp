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

void Maike::Db::compile(SourceTree const& src_tree, Sched::ThreadPool& workers)
{
	visitNodesInTopoOrder([&graph = src_tree.dependencyGraph(),
	                       ctxt = makeCompilationContext(src_tree.dependencyGraph(), workers)](
	                         auto const& node) mutable { compile(graph, node, ctxt); },
	                      src_tree.dependencyGraph());
}

void Maike::Db::compile(SourceTree const& src_tree,
                        fs::path const& target_name,
                        Sched::ThreadPool& workers)
{
	auto const& target = getTarget(src_tree, target_name);
	auto const& graph = src_tree.dependencyGraph();
	auto const& node = getNode(graph, target.sourceFilename());

	processGraphNodeRecursive([&graph = src_tree.dependencyGraph(),
	                           ctxt = makeCompilationContext(src_tree.dependencyGraph(), workers)](
	                             auto const& node) mutable { compile(graph, node, ctxt); },
	                          graph,
	                          node);
}

void Maike::Db::compileAlways(SourceTree const& src_tree,
                              fs::path const& target_name,
                              Sched::ThreadPool& workers)
{
	auto const& target = getTarget(src_tree, target_name);
	auto const& graph = src_tree.dependencyGraph();
	auto const& node = getNode(graph, target.sourceFilename());

	processGraphNodeRecursive([&graph = src_tree.dependencyGraph(),
	                           ctxt = makeCompilationContext(src_tree.dependencyGraph(), workers)](
	                             auto const& node) mutable { compileAlways(graph, node, ctxt); },
	                          graph,
	                          node);
}

void Maike::Db::compileAlways(SourceTree const& src_tree, Sched::ThreadPool& workers)
{
	visitNodesInTopoOrder([&graph = src_tree.dependencyGraph(),
	                       ctxt = makeCompilationContext(src_tree.dependencyGraph(), workers)](
	                         auto const& node) mutable { compileAlways(graph, node, ctxt); },
	                      src_tree.dependencyGraph());
}