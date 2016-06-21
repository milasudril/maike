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
	,std::vector<std::pair<Maike::Target*, Maike::Dependency::Relation> >& targets
	,size_t n_targets)
	{
	std::vector<int> visited(n_targets,0);
	struct Node
		{
		Maike::Target* target;
		Maike::Dependency::Relation rel;
		bool done;
		};

	std::stack< Node > nodes;

	nodes.push({&target_first,Maike::Dependency::Relation::LEAF,0});

	while(!nodes.empty())
		{
		auto node_current=nodes.top();
		nodes.pop();
		auto target=node_current.target;
		if(node_current.done)
			{
			visited[target->idGet()]=2;
			targets.push_back({node_current.target,node_current.rel});
			continue;
			}
		nodes.push({target,node_current.rel,1});
		visited[target->idGet()]=1;
		auto deps=target->dependencies();
		while(deps.first!=deps.second)
			{
			auto target_next=deps.first->target();
			if(target_next==nullptr)
				{
				throw __FILE__;
			//TODO throw ErrorMessage("#0;: Undefined reference to #1;."
				//	,{target_current.nameSourceGet(),pos->nameGet()});
				}
			if(visited[target_next->idGet()])
				{
				throw __FILE__;
			//TODO throw ErrorMessage("A cyclic dependency between #0; and #1; was detected."
				//	,{target_current.nameGet(),target_next->nameGet()});
				}
			nodes.push({deps.first->target(),deps.first->relationGet(),0});
			++(deps.first);
			}
		}

	}


int main(int argc,char** args)
	{
	Maike::DependencyGraphDefault graph;

	std::vector<Maike::Target*> leafs;
	graph.targetsPatch().targetsProcess(LeafCollector(leafs));

	auto leafs_begin=leafs.data();
	auto leafs_end=leafs_begin + leafs.size();
	while(leafs_begin!=leafs_end)
		{
		std::vector<std::pair<Maike::Target*, Maike::Dependency::Relation> > targets;
		toposort(*leafs[0],targets,graph.targetCounterGet());
		++leafs_end;
		}

	return 0;
	}
