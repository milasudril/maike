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

#if TARGETS_LOAD_DONE //To get syntax highlight
void targetsLoad(DependencyGraph& graph,Twins<const Target_Hook*> hooks)
	{
	Maike::ExpressionEvaluatorDefault evaluator(targetinfo);
	Target_FactoryDelegatorDefault delegator(
		static_cast<const char*>(targetinfo.variableGet(Maike::Stringkey("target_directory"))),evaluator);

	SpiderDefault spider(delegator,graph);

	while(hooks.first!=hooks.second)
		{
		delegator.factoryRegister(hooks.first->filename_ext,hooks.first->factory);
		spider.loaderRegister(hooks.first->filename_ext,hooks.first->loader);
		++hooks.first;
		}

	spider.scanFile(".","").run();
	}

#endif

static void toposort(const Maike::Dependency& dependency_first
	,std::vector<Maike::Dependency>& dependency_list
	,bool full
	,std::vector<uint8_t>& visited
	,size_t id_min)
	{
	auto& target_first=*dependency_first.target();
	auto id_first=target_first.idGet() - id_min;
	visited[id_first]=1;

	auto deps=target_first.dependencies();
	while(deps.first!=deps.second)
		{
		auto target_next=deps.first->target();
		auto id=target_next->idGet() - id_min;
		if(visited[id]==1)
			{
			exceptionRaise(ErrorMessage("A cyclic dependency between #0; and #1; was detected."
				,{target_first.nameGet(),target_next->nameGet()}));
			}

		if(visited[id]==0 && (full
			|| deps.first->relationGet()!=Dependency::Relation::IMPLEMENTATION))
			{toposort(*deps.first,dependency_list,full,visited,id_min);}
		++(deps.first);
		}
	visited[id_first]=2;
	dependency_list.push_back(dependency_first);
	}

static void toposort(Target& target_first
	,std::vector<Maike::Dependency>& dependency_list,const Twins<size_t>& id_range
	,bool full)
	{
	std::vector<uint8_t> visited((id_range.second-id_range.first) + 1,0);
	toposort(Dependency(target_first),dependency_list,full,visited,id_range.first);
	}

void Maike::buildBranch(Target& target,const char* target_dir
	,const Twins<size_t>& id_range)
	{
	std::vector<Dependency> dependency_list_full;
	toposort(target,dependency_list_full,id_range,1);

	auto deps_begin=dependency_list_full.data();
	Twins<Dependency*> deps(deps_begin,deps_begin + dependency_list_full.size());

	while(deps.first!=deps.second)
		{
		Twins<const Dependency*> deps_rel_full(deps_begin,deps.first);
		auto target=deps.first->target();
		if(target!=nullptr)
			{
			std::vector<Dependency> dependency_list;
			toposort(*target,dependency_list,id_range,0);
			Twins<const Dependency*> deps_rel(dependency_list.data()
				,dependency_list.data() + dependency_list.size());

			if(!target->upToDate(deps_rel,deps_rel_full,target_dir))
				{target->compile(deps_rel,deps_rel_full,target_dir);}
			}
		++deps.first;
		}
	}
