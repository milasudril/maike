//@	{
//@	  "targets":[{"name":"graphutils.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_GRAPHUTILS_HPP
#define MAIKE_GRAPHUTILS_HPP

#include <vector>
#include <cstddef>
#include <stack>
#include <stdexcept>
#include <type_traits>

namespace Maike
{
	namespace graphutils_detail
	{
		enum class Mark : int
		{
			Init,
			InProgress,
			Done
		};

		template<class T>
		inline bool valid(T x, std::enable_if_t<std::is_integral_v<T>, int> = 0)
		{
			return x != static_cast<T>(-1);
		}

		template<class ItemCallback, class Graph, class... VisitEdgesArgs>
		void processGraphNodeRecursive(ItemCallback&& cb,
		                               Graph const& graph,
		                               typename Graph::node_type const& node,
		                               std::vector<Mark>& visited,
		                               VisitEdgesArgs... visit_edges_args)
		{
			using Node = typename Graph::node_type;
			using Edge = typename Graph::edge_type;
			Edge const* incoming_edge{nullptr};
			std::stack<std::pair<Node const*, Edge const*>> nodes_to_visit;
			switch(visited[static_cast<size_t>(id(node))])
			{
				case Mark::Init:
					nodes_to_visit.push(std::pair{&node, incoming_edge});
					while(!nodes_to_visit.empty())
					{
						auto node_edge = nodes_to_visit.top();
						auto node_id = id(*node_edge.first);
						switch(visited[static_cast<size_t>(node_id)])
						{
							case Mark::Init:
							{
								auto processEdge = [&graph, &nodes_to_visit, &visited](auto const& edge) {
									auto node_id = reference(edge);
									if(valid(node_id))
									{
										switch(visited[static_cast<size_t>(node_id)])
										{
											case Mark::Init:
												nodes_to_visit.push(std::pair{&getNodeById(graph, node_id), &edge});
												break;

											case Mark::InProgress: throw std::runtime_error{"Cyclic dependency detected"};

											case Mark::Done: break;
										}
									}
								};
								visited[static_cast<size_t>(node_id)] = Mark::InProgress;
								visitEdges(processEdge, *node_edge.first, visit_edges_args...);
								break;
							}
							case Mark::InProgress:
								visited[static_cast<size_t>(node_id)] = Mark::Done;
								cb(*node_edge.first, node_edge.second);
								nodes_to_visit.pop();
								break;
							default: nodes_to_visit.pop(); break;
						}
					}
					break;
				case Mark::InProgress: throw std::runtime_error{"Cyclic dependency detected"};
				case Mark::Done: return;
			}
		}
	}

	template<class ItemCallback, class Graph, class Node, class... VisitEdgesArgs>
	void processGraphNodeRecursive(ItemCallback&& cb,
	                               Graph const& graph,
	                               Node const& node,
	                               VisitEdgesArgs&&... args)
	{
		using std::size;
		auto const N = size(graph);
		std::vector<graphutils_detail::Mark> visited(N);
		graphutils_detail::processGraphNodeRecursive(
		   cb, graph, node, visited, std::forward<VisitEdgesArgs>(args)...);
	}

	template<class ItemCallback, class Graph>
	void visitNodesInTopoOrder(ItemCallback&& cb, Graph const& graph)
	{
		using std::size;
		auto const N = size(graph);
		std::vector<graphutils_detail::Mark> visited(N);

		auto processNode = [&cb, &graph, &visited](auto const& node) {
			graphutils_detail::processGraphNodeRecursive(cb, graph, node, visited);
		};

		visitNodes(processNode, graph);
	}
}

#endif