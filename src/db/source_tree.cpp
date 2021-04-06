//@	{
//@	 "targets":[{"name":"source_tree.o","type":"object"}]
//@	}

#include "./source_tree.hpp"

#include "src/utils/graphutils.hpp"

void Maike::Db::compile(SourceTree const& src_tree)
{
	visitNodesInTopoOrder(
	   [&graph = src_tree.dependencyGraph()](auto const& node) {
		   auto use_deps = getUseDepsRecursive(graph, node);
		   if(!isUpToDate(node, use_deps)) { compile(node, use_deps); }
	   },
	   src_tree.dependencyGraph());
}

void Maike::Db::compileAlways(SourceTree const& src_tree, fs::path const& target)
{
	auto const& targets = src_tree.targets();
	auto i = targets.find(target);
	if(i == std::end(targets))
	{
		std::string msg{"Unkonwn target `"};
		msg += target;
		msg += "`";
		throw std::runtime_error{std::move(msg)};
	}

	auto const& graph = src_tree.dependencyGraph();
	auto const& node = getNode(graph, i->second.sourceFilename());

	processGraphNodeRecursive(
	   [&graph = src_tree.dependencyGraph()](auto const& node) {
		   compile(node, getUseDepsRecursive(graph, node));
	   },
	   graph,
	   node);
}

void Maike::Db::compileAlways(SourceTree const& src_tree)
{
	visitNodesInTopoOrder([&graph = src_tree.dependencyGraph()](
	                         auto const& node) { compile(node, getUseDepsRecursive(graph, node)); },
	                      src_tree.dependencyGraph());
}

void Maike::Db::compile(SourceTree const& src_tree, fs::path const& target);