//@ {
//@ "targets":[{"name":"maike","type":"application"}]
//@ }

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "dependency.hpp"
#include <vector>
#include <stack>

class LeafCollector:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		LeafCollector(std::vector<Maike::Target*>& leafs):r_leafs(leafs)
			{}

		void operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			if(target_current.childCounterGet()==0)
				{r_leafs.push_back(&target_current);}
			}

	private:
		std::vector<Maike::Target*>& r_leafs;
	};

void toposort(Maike::Target& target_first
	,std::vector<Maike::Dependency>& dependency_list,size_t targets_count)
	{
	std::vector<int> visited(targets_count,0);
	struct Node
		{
		Maike::Dependency* dependency;
		bool done;
		};

	std::stack< Node > nodes;

	Maike::Dependency dep_first(target_first);
	nodes.push({&dep_first,0});

	while(!nodes.empty())
		{
		auto node_current=nodes.top();
		nodes.pop();
		auto dep=node_current.dependency;
		if(node_current.done)
			{
			if(dep->target()!=nullptr)
				{visited[dep->target()->idGet()]=2;}
			dependency_list.push_back(*dep);
			continue;
			}

	//	Node is a "child", more work to do.
		nodes.push({dep,1}); //	After all child nodes have been processed, this node is done
		auto target_current=dep->target();
		visited[target_current->idGet()]=1;
		auto deps=target_current->dependencies();
		while(deps.first!=deps.second)
			{
			auto target_next=deps.first->target();
		//	Empty target
			if(target_next==nullptr)
				{
			//	Only external references are allowed to have an empty target
				if(deps.first->relationGet()!=Maike::Dependency::Relation::EXTERNAL)
					{
					throw __FILE__;
				//TODO throw ErrorMessage("#0;: Undefined reference to #1;."
					//	,{target_current.nameSourceGet(),deps.first->nameGet()});
					}
			//	Push the node and continue. Since there are no children, mark
			//	this node as done.
				nodes.push({deps.first,1});
				}
			else //Non-empty target
			if(visited[target_next->idGet()]==1)
				{
				throw __FILE__;
			//TODO throw ErrorMessage("A cyclic dependency between #0; and #1; was detected."
				//	,{target_current.nameGet(),target_next->nameGet()});
				}
			nodes.push({deps.first,0});
			++(deps.first);
			}
		}

	}

void build(Maike::Target& target,Maike::Invoker& invoker,size_t targets_count)
	{
	std::vector<Maike::Dependency> dependency_list;
	toposort(target,dependency_list,targets_count);
	Maike::Twins<const Maike::Dependency*> deps;
	deps.first=dependency_list.data();
	deps.second=deps.first + dependency_list.size();
	if(!target.upToDate(deps,invoker))
		{target.compile(deps,invoker);}
	}

void buildAll(Maike::Twins<Maike::Target*> leafs,Maike::Invoker& invoker
	,size_t targets_count)
	{
	while(leafs.first!=leafs.second)
		{
		build(*leafs.first,invoker,targets_count);
		++(leafs.first);
		}
	}




int main(int argc,char** args)
	{
	Maike::DependencyGraphDefault graph;

	std::vector<Maike::Target*> leafs;
	graph.targetsPatch().targetsProcess(LeafCollector(leafs));
//	buildAll(leafs,exec_invoker,graph.targetCounterGet());

	return 0;
	}
