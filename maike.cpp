//@	{
//@	"targets":[{"name":"maike.o","type":"object"}]
//@	}

#include "maike.hpp"
#include "target.hpp"
#include "dependency.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include <vector>
#include <stack>

using namespace Maike;

static void toposort(Maike::Target& target_first
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
					exceptionRaise(ErrorMessage("#0;: Undefined reference to #1;."
						,{target_current->sourceNameGet(),deps.first->nameGet()}));
					}
			//	Push the node and continue. Since there are no children, mark
			//	this node as done.
				nodes.push({deps.first,1});
				}
			else //Non-empty target
			if(visited[target_next->idGet()]==1)
				{
				exceptionRaise(ErrorMessage("A cyclic dependency between #0; and #1; was detected."
					,{target_current->nameGet(),target_next->nameGet()}));
				}
			nodes.push({deps.first,0});
			++(deps.first);
			}
		}

	}

void Maike::buildBranch(Target& target,const char* target_dir,size_t targets_count)
	{
	std::vector<Dependency> dependency_list;
	toposort(target,dependency_list,targets_count);

	auto deps_begin=dependency_list.data();
	Twins<Dependency*> deps(deps_begin,deps_begin + dependency_list.size());

	while(deps.first!=deps.second)
		{
		Twins<const Dependency*> deps_rel(deps_begin,deps.first);
		auto target=deps.first->target();
		if(target!=nullptr)
			{
			if(!target->upToDate(deps_rel,target_dir))
				{target->compile(deps_rel,target_dir);}
			}
		++deps.first;
		}
	}
