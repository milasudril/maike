//@	{
//@	  "targets":[{"name":"graphutils.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_GRAPHUTILS_HPP
#define MAIKE_GRAPHUTILS_HPP

#include <vector>
#include <cstddef>
#include <stack>
#include <stdexcept>

namespace Maike
{
	template<class ItemCallback, class Graph>
	void visitNodes(ItemCallback&& cb, Graph const& graph)
	{
		using std::size;
		enum class Mark : int
		{
			Init,
			InProgress,
			Done
		};

		auto const N = size(graph);
		std::vector<Mark> visited(N);
		std::stack<typename Graph::node_type const*> nodes;


		auto processNode = [&nodes, &visited, &graph, &cb](auto const& node) {
			switch(visited[id(node)])
			{
				case Mark::Init:
				{
					auto processEdge = [&visited, &nodes, &graph](auto node_id) {
						switch(visited[node_id])
						{
							case Mark::Init: nodes.push(getNodeById(graph, node_id)); break;

							case Mark::InProgress: throw std::runtime_error{"Cyclic dependency detected"};

							case Mark::Done:
							break;
						}
					};

					nodes.push(&node);
					while(!nodes.empty())
					{
						auto node = nodes.top();
						auto node_id = id(*node);
						switch(visited[node_id])
						{
							case Mark::Init:
								visited[node_id] = Mark::InProgress;
								visitEdges(processEdge, *node);
								break;
							default:
								visited[node_id] = Mark::Done;
								cb(*node);
								nodes.pop();
						}
					}
				}
				break;
				case Mark::InProgress: throw std::runtime_error{"Cyclic dependency detected"};

				case Mark::Done:
				break;

			}
		};

		visitNodes(processNode, graph);
	}
}

#endif