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
#include "writebuffer.hpp"
#include "maikeinfo.hpp"

#include <vector>
#include <stack>

using namespace Maike;


static void versionPrintImpl(DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("Maike version ").write(Info::VERSION)
		.write("\nThis Maike was compiled on ").write(Info::TIMESTAMP).write('\n');
	}


void Maike::versionPrint(DataSink&& sink)
	{::versionPrintImpl(sink);}

void Maike::versionPrint(DataSink& sink)
	{::versionPrintImpl(sink);}


static void toposort(const Maike::Dependency& dependency_first
	,std::vector<Maike::Dependency>& dependency_list
	,bool full
	,std::vector<int>& visited)
	{
	auto& target_first=*dependency_first.target();
	auto id_first=target_first.idGet();
	visited[id_first]=1;

	auto deps=target_first.dependencies();
	while(deps.first!=deps.second)
		{
		auto target_next=deps.first->target();
		auto id=target_next->idGet();
		if(visited[id]==1)
			{
			exceptionRaise(ErrorMessage("A cyclic dependency between #0; and #1; was detected."
				,{target_first.nameGet(),target_next->nameGet()}));
			}

		if(visited[id]==0 && (full
			|| deps.first->relationGet()!=Dependency::Relation::IMPLEMENTATION))
			{toposort(*deps.first,dependency_list,full,visited);}
		++(deps.first);
		}
	visited[id_first]=2;
	dependency_list.push_back(dependency_first);
	}

static void toposort(Target& target_first
	,std::vector<Maike::Dependency>& dependency_list,size_t targets_count
	,bool full)
	{
	std::vector<int> visited(targets_count,0);
	toposort(Dependency(target_first),dependency_list,full,visited);
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
