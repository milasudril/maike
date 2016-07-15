//@	{
//@	"targets":[{"name":"maike.o","type":"object"}]
//@	}

#include "maike.hpp"
#include "maikeinfo.hpp"
#include "targetplaceholder.hpp"
#include "dependency.hpp"
#include "session.hpp"
#include "stringkey.hpp"

#include "writebuffer.hpp"
#include "resourceobject.hpp"
#include "graphedgewriter.hpp"

#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"

#include <vector>
#include <stack>
#include <cstring>

using namespace Maike;


void Maike::versionPrint(TextWriter& writer)
	{
	writer.write("Maike version ").write(Info::VERSION)
		.write("\nThis Maike was compiled on ").write(Info::TIMESTAMP).write("\n");
	}



void Maike::configDump(const Session& maike,DataSink& sink)
	{
	ResourceObject obj(ResourceObject::Type::OBJECT);
	maike.configDump(obj);
	obj.write(sink);
	}



void Maike::configAppendDefault(Session& maike,DataSource& source)
	{
	ResourceObject obj(source);
	maike.configAppend(obj);
	}

void Maike::configAppendDefault(Session& maike)
	{
	maike.configAppendDefault();
	}



namespace
	{
	class TargetsListAll:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListAll(TextWriter& writer):r_writer(writer)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				r_writer.write(" * ").write(target.nameGet()).write("\n");
				return 0;
				}
		private:
			TextWriter& r_writer;
		};
	}

void Maike::targetsListAll(const Session& maike,TextWriter& writer)
	{
	writer.write("All targets\n")
		.write("===========\n");
	maike.targetsProcess(TargetsListAll(writer));
	}



namespace
	{
	class TargetsListLeaf:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListLeaf(TextWriter& writer):r_writer(writer)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				if(target.childCounterGet()==0)
					{r_writer.write(" * ").write(target.nameGet()).write("\n");}
				return 0;
				}
		private:
			TextWriter& r_writer;
		};
	}

void Maike::targetsListLeaf(const Session& maike,TextWriter& writer)
	{
	writer.write("Leaf targets\n")
		.write("============\n");
	maike.targetsProcess(TargetsListLeaf(writer));
	}



namespace
	{
	class TargetsListExternal:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListExternal(TextWriter& writer):r_writer(writer)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				if(dynamic_cast<const TargetPlaceholder*>(&target))
					{r_writer.write(" * ").write(target.nameGet()).write("\n");}
				return 0;
				}
		private:
			TextWriter& r_writer;
		};
	}

void Maike::targetsListExternal(const Session& maike,TextWriter& writer)
	{
	writer.write("External dependencies\n")
		.write("=====================\n");
	maike.targetsProcess(TargetsListExternal(writer));
	}



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
			exceptionRaise(ErrorMessage("A cyclic dependency between #0;[#2;] and #1;[#3;] was detected."
				,{target_first.nameGet(),target_next->nameGet(),target_first.idGet(),target_next->idGet()}));
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

static void buildBranch(Target& target,const char* target_dir
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

void Maike::targetCompile(Session& maike,const char* target_name)
	{
	auto& target=maike.target(target_name);
	auto target_dir=maike.targetDirectoryGet();
	auto& id_range=maike.targetIdRangeGet();
	buildBranch(target,target_dir,id_range);
	}

namespace
	{
	class TargetsCompileLeaf:public DependencyGraph::TargetProcessor
		{
		public:
			TargetsCompileLeaf(const Session& maike):
				r_target_dir(maike.targetDirectoryGet())
				,id_range(maike.targetIdRangeGet())
				{}

			int operator()(DependencyGraph& graph,Target& target)
				{
				if(target.childCounterGet()==0)
					{buildBranch(target,r_target_dir,id_range);}
				return 0;
				}
		private:
			const char* r_target_dir;
			Twins<size_t> id_range;
		};
	}

void Maike::targetsCompile(Session& maike)
	{
	maike.targetsProcess(TargetsCompileLeaf(maike));
	}



namespace
	{
	class GraphEdgesWrite:public DependencyGraph::TargetProcessorConst
		{
		public:
			GraphEdgesWrite(GraphEdgeWriter& writer):r_writer(writer)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				auto name_target=target.nameGet();
				if(strcmp(name_target,".")!=0)
					{
					auto dep=target.dependencies();
					while(dep.first!=dep.second)
						{
						auto name_dep=dep.first->nameGet();
						if(strcmp(name_dep,".")!=0)
							{
							r_writer.edgeWrite(name_target,dep.first->nameGet()
								,dep.first->relationGet()==Dependency::Relation::IMPLEMENTATION
									?"red":"blue");
							}
						++dep.first;
						}
					}
				return 0;
				}
		private:
			GraphEdgeWriter& r_writer;
		};
	}

void Maike::graphDump(const Session& maike,GraphEdgeWriter& writer)
	{
	maike.targetsProcess(GraphEdgesWrite(writer));
	}

void Maike::graphDump(const Session& maike,GraphEdgeWriter& writer
	,const char* target_start,uint8_t* targets_visited,size_t id_min)
	{
	std::stack<const Target*> nodes;
	nodes.push(&maike.target(target_start));
	while(!nodes.empty())
		{
		auto node_current=nodes.top();
		nodes.pop();
		auto id=node_current->idGet()-id_min;
		if(targets_visited[id]==0)
			{
			auto name_target=node_current->nameGet();
			auto dep=node_current->dependencies();
			while(dep.first!=dep.second)
				{
				auto name_dep=dep.first->nameGet();
				if(strcmp(name_dep,".")!=0)
					{
					writer.edgeWrite(name_target,name_dep
						,dep.first->relationGet()==Dependency::Relation::IMPLEMENTATION
							?"red":"blue");
					nodes.push(dep.first->target());
					}
				++dep.first;
				}
			targets_visited[id]=1;
			}
		}
	}

void Maike::targetDump(const Session& maike,ResourceObject& db
	,const char* target_name)
	{
	ResourceObject target_obj(ResourceObject::Type::OBJECT);
	maike.target(target_name).dump(target_obj);
	db.objectAppend(std::move(target_obj));
	}



namespace
	{
	class TargetDumpJSON:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetDumpJSON(ResourceObject& db):r_db(db)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				ResourceObject target_obj(ResourceObject::Type::OBJECT);
				target.dump(target_obj);
				r_db.objectAppend(std::move(target_obj));
				return 0;
				}
		private:
			ResourceObject& r_db;
		};
	}

void Maike::targetsDump(const Session& maike,ResourceObject& db)
	{
	maike.targetsProcess(TargetDumpJSON(db));
	}



void Maike::targetsDumpTSVHeader(TextWriter& writer)
	{
	writer.write("name\t")
		.write("source_name\t")
		.write("child_count\t")
		.write("dependency_count\t")
		.write("compilation_time\t")
		.write("lines_of_code\n");
	}


static void targetDumpTSV(const Target& target,TextWriter& writer)
	{
	auto deps=target.dependencies();
	writer.write(target.nameGet()).write("\t")
		.write(target.sourceNameGet()).write("\t")
		.write(target.childCounterGet()).write("\t")
		.write(static_cast<size_t>(deps.second-deps.first)).write("\t")
		.write(target.compilationTimeGet()).write("\t")
		.write(target.lineCountGet()).write("\n");
	}

namespace
	{
	class TargetDumpTSV:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetDumpTSV(TextWriter& writer):r_writer(writer)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				::targetDumpTSV(target,r_writer);
				return 0;
				}
		private:
			TextWriter& r_writer;
		};
	}

void Maike::targetsDumpTSV(const Session& maike,TextWriter& writer)
	{
	maike.targetsProcess(TargetDumpTSV(writer));
	}

void Maike::targetDumpTSV(const Session& maike,TextWriter& writer,const char* target_name)
	{
	::targetDumpTSV(maike.target(target_name),writer);
	}

