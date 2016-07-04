//@	{
//@	"targets":[{"name":"maike.o","type":"object"}]
//@	}

#include "maike.hpp"
#include "target.hpp"
#include "dependency.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "stringkey.hpp"
#include <vector>
#include <stack>
#include <set>

using namespace Maike;

static void toposort(Target& target_first
	,std::vector<Maike::Dependency>& dependency_list,size_t targets_count
	,bool full)
	{
	std::vector<int> visited(targets_count,0);
	std::set<Stringkey> visited_external;
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
				{
				assert(visited[dep->target()->idGet()]==1);
				visited[dep->target()->idGet()]=2;
				}
			if(full)
				{printf("toposort: %s\n",dep->nameGet());}
			dependency_list.push_back(*dep);
			continue;
			}

		auto target_current=dep->target();
		if(target_current==nullptr) //Null from external dependency
			{continue;}

		auto target_id=target_current->idGet();

	//	Node is a "child", more work to do.
		nodes.push({dep,1}); //	After all child nodes have been processed, this node is done
		visited[target_id]=1;
		auto deps=target_current->dependencies();
		while(deps.first!=deps.second)
			{
			auto target_next=deps.first->target();
		//	Empty target
			if(target_next==nullptr)
				{
			//	Only external references are allowed to have an empty target
			/*	if(deps.first->relationGet()!=Dependency::Relation::EXTERNAL)
					{
					exceptionRaise(ErrorMessage("#0;: Undefined reference to #1;."
						,{target_current->sourceNameGet(),deps.first->nameGet()}));
					}

				if(visited_external.insert(Stringkey(deps.first->nameGet())).second)
					{
				//	Push the node and continue. Since there are no children, mark
				//	this node as done.
					nodes.push({deps.first,1});
					}*/
				}
			else//Non-empty target
				{
				target_id=target_next->idGet();
				if(visited[target_id]==1)
					{
					exceptionRaise(ErrorMessage("A cyclic dependency between #0; and #1; was detected."
						,{target_current->nameGet(),target_next->nameGet()}));
					}
				if(visited[target_id]==0 && (full
				|| deps.first->relationGet()!=Dependency::Relation::IMPLEMENTATION))
					{
					assert(visited[target_id]==0);
					nodes.push({deps.first,0});
					}
				}
			++(deps.first);
			}
		}
	}

void Maike::buildBranch(Target& target,const char* target_dir,size_t targets_count)
	{
	std::vector<Dependency> dependency_list_full;
	toposort(target,dependency_list_full,targets_count,1);

	auto deps_begin=dependency_list_full.data();
	Twins<Dependency*> deps(deps_begin,deps_begin + dependency_list_full.size());

	while(deps.first!=deps.second)
		{
		Twins<const Dependency*> deps_rel_full(deps_begin,deps.first);
		auto target=deps.first->target();
		if(target!=nullptr)
			{
			std::vector<Dependency> dependency_list;
			toposort(*target,dependency_list,targets_count,0);
			Twins<const Dependency*> deps_rel(dependency_list.data()
				,dependency_list.data() + dependency_list.size());

			if(!target->upToDate(deps_rel,deps_rel_full,target_dir))
				{target->compile(deps_rel,deps_rel_full,target_dir);}
			}
		++deps.first;
		}
	}
