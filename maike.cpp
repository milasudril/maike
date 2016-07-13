//@	{
//@	"targets":[{"name":"maike.o","type":"object"}]
//@	}

#include "maike.hpp"
#include "maikeinfo.hpp"
#include "targetplaceholder.hpp"
#include "dependency.hpp"
#include "session.hpp"
#include "stringkey.hpp"

#include "filein.hpp"
#include "fileout.hpp"
#include "writebuffer.hpp"
#include "resourceobject.hpp"

#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"

#include <vector>
#include <stack>

using namespace Maike;


void Maike::versionPrint(DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("Maike version ").write(Info::VERSION)
		.write("\nThis Maike was compiled on ").write(Info::TIMESTAMP).write('\n');
	}

void Maike::versionPrint(const char* filename)
	{
	FileOut dest(filename);
	versionPrint(dest);
	}



void Maike::configDump(const Session& maike,DataSink& sink)
	{
	ResourceObject obj(ResourceObject::Type::OBJECT);
	maike.configDump(obj);
	obj.write(sink);
	}

void Maike::configDump(const Session& maike,const char* filename)
	{
	FileOut dest(filename);
	configDump(maike,dest);
	}



void Maike::configLoad(Session& maike,DataSource& source)
	{
	ResourceObject obj(source);
	maike.configAppend(obj);
	}

void Maike::configLoad(Session& maike,const char* filename)
	{
	FileIn source(filename);
	configLoad(maike,source);
	}



namespace
	{
	class TargetsListAll:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListAll(WriteBuffer& wb):r_wb(wb)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				r_wb.write(" * ").write(target.nameGet()).write("\n");
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsListAll(const Session& session,DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("All targets\n")
		.write("===========\n");
	session.targetsProcess(TargetsListAll(wb));
	}

void Maike::targetsListAll(const Session& session,const char* filename)
	{
	FileOut dest(filename);
	targetsListAll(session,dest);
	}



namespace
	{
	class TargetsListLeaf:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListLeaf(WriteBuffer& wb):r_wb(wb)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				if(target.childCounterGet()==0)
					{r_wb.write(" * ").write(target.nameGet()).write("\n");}
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsListLeaf(const Session& session,DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("Leaf targets\n")
		.write("============\n");
	session.targetsProcess(TargetsListLeaf(wb));
	}

void Maike::targetsListLeaf(const Session& session,const char* filename)
	{
	FileOut dest(filename);
	targetsListLeaf(session,dest);
	}



namespace
	{
	class TargetsListExternal:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListExternal(WriteBuffer& wb):r_wb(wb)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				if(dynamic_cast<const TargetPlaceholder*>(&target))
					{r_wb.write(" * ").write(target.nameGet()).write("\n");}
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsListExternal(const Session& session,DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("External dependencies\n")
		.write("=====================\n");
	session.targetsProcess(TargetsListExternal(wb));
	}

void Maike::targetsListExternal(const Session& session,const char* filename)
	{
	FileOut dest(filename);
	targetsListExternal(session,dest);
	}



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
