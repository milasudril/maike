//@	{
//@	"targets":[{"name":"maike.o","type":"object"}]
//@	}

#include "maike_base.hpp"
#include "targetinclude.hpp"
#include MAIKE_TARGET(projectinfo.hpp)
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
#include "stdstream.hpp"
#include "resourceobjectjansson.hpp"
#include "filein.hpp"
#include "fileout.hpp"
#include "fileutils.hpp"
#include "readbuffer.hpp"

#include <vector>
#include <stack>
#include <cstring>
#include <array>

using namespace Maike;


void Maike::versionPrint(DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write(ProjectInfo::name()).write(" version ").write(ProjectInfo::version())
		.write("\nThis Maike was compiled on ").write(ProjectInfo::timestamp())
		.write("\n");
	}

void Maike::about(DataSink& sink)
	{
	versionPrint(sink);
	WriteBuffer wb(sink);
	wb.write("Copyright (C) ").write(ProjectInfo::yearString()).write(" ")
		.write(ProjectInfo::author()).write("\n\n")
		.write(ProjectInfo::legalBrief()).write("\n");
	}

void Maike::loadPath(DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write(dirname(exename()).c_str());
	}

ResourceObject Maike::resourceObjectCreate(ResourceObject::Type type)
	{
	ResourceObjectJansson ret(type);
	return std::move(ret);
	}

ResourceObject Maike::resourceObjectCreate(DataSource& src)
	{
	ResourceObjectJansson ret(src);
	return std::move(ret);
	}

void Maike::init(ExceptionHandler eh)
	{exceptionHandlerSet(eh);}

void Maike::init(DataSink& standard_output,DataSink& standard_error)
	{
	StdStream::outputSet(standard_output);
	StdStream::errorSet(standard_error);
	}

void Maike::init(DataSink& standard_output,DataSink& standard_error
	,ExceptionHandler eh)
	{
	init(eh);
	init(standard_output,standard_error);
	}

static bool findString(const char* filename,const char* str)
	{
	FileIn src(filename);
	ReadBuffer buffer(src);
	auto pos_init=reinterpret_cast<const uint8_t*>(str);
	auto pos_current=pos_init;
	while(!buffer.eof())
		{
		auto ch_in=buffer.byteRead();
		auto v_str=*pos_current;
		if(v_str=='\0')
			{return 1;}
		if(v_str==ch_in)
			{++pos_current;}
		else
			{
			pos_current=pos_init;
			if(*pos_current==ch_in)
				{++pos_current;}
			}
		}
	return 0;
	}

static bool isMaike()
	{
	static bool checked=0;
	if(checked)
		{return 1;}
	auto filename=exename();
	checked=1;
	return findString(filename.c_str(),ProjectInfo::name())
		&& findString(filename.c_str(),ProjectInfo::legalBrief())
		&& findString(filename.c_str(),ProjectInfo::author());
	}

Session* Maike::sessionCreateRaw()
	{
	if(!isMaike())
		{about(StdStream::error());}
	return new Session;
	}

void Maike::sessionDestroy(Session* maike)
	{delete maike;}



void Maike::configDump(const Session& maike,DataSink& sink)
	{
	configDump(maike).write(sink);
	}

ResourceObject Maike::configDump(const Session& maike)
	{
	ResourceObjectJansson obj(ResourceObject::Type::OBJECT);
	maike.configDump(obj);
	return std::move(obj);
	}


void Maike::configAppend(Session& maike,DataSource& source)
	{
	ResourceObjectJansson obj(source);
	maike.configAppend(obj);
	}

void Maike::configAppend(Session& maike,const ResourceObject& obj)
	{maike.configAppend(obj);}

void Maike::hookRegister(Session& maike,const char* name_plugin
	,Twins<const char* const*> filename_exts)
	{maike.hookRegister(name_plugin,filename_exts);}

void Maike::hookConfigAppend(Session& maike,const char* name_hook,const ResourceObject& rc)
	{maike.hookConfigAppend(name_hook,rc);}

void Maike::configAppendDefault(Session& maike)
	{maike.configAppendDefault();}

void Maike::configAppendCommon(Session& maike)
	{
	std::array<std::string,3> dirs{configdir(),configdirUser(),getcwd()};
	WriteBuffer wb(StdStream::error());
	for(size_t k=0;k<dirs.size();++k)
		{
		try
			{
			FileIn source(dircat(dirs[k],"maikeconfig.json").c_str());
			ResourceObjectJansson obj(source);
			maike.configAppend(obj);
			}
		catch(const ErrorMessage& msg)
			{
			wb.write(msg.messageGet()).write(". Continuing search...\n");
			}
		}
	}

void Maike::configClear(Session& maike)
	{maike.configClear();}

void Maike::sysvarsLoad(Session& maike)
	{maike.sysvarsLoad();}

void Maike::rootSet(Session& maike,const char* root)
	{maike.rootSet(root);}

void Maike::scanFile(Session& maike,const char* file)
	{maike.scanFile(file);}

void Maike::scanRecursiveSet(Session& maike,bool recursive)
	{maike.scanRecursiveSet(recursive);}

bool Maike::scanRecursiveGet(const Session& maike)
	{return maike.scanRecursiveGet();}

const char* Maike::targetDirectoryGet(const Session& maike)
	{return maike.targetDirectoryGet();}

bool Maike::loaderHas(const Session& maike,const char* filename)
	{return maike.loaderHas(filename);}

Twins<size_t> Maike::targetIdRangeGet(const Session& maike)
	{return maike.targetIdRangeGet();}



namespace
	{
	class TargetsListAll:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsListAll(WriteBuffer& wb):r_wb(wb)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				r_wb.write(" * ").write(target.nameGet());
				auto description=target.descriptionGet();
				if(*description!='\0')
					{r_wb.write(" -- ").write(description);}
				r_wb.write("\n");
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsListAll(const Session& maike,DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("All targets\n")
		.write("===========\n");
	maike.targetsProcess(TargetsListAll(wb));
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
				auto x=target.dependenciesInverseGet();
				if(x.second - x.first==0)
					{
					r_wb.write(" * ").write(target.nameGet());
					auto description=target.descriptionGet();
					if(*description!='\0')
						{r_wb.write(" -- ").write(description);}
					r_wb.write("\n");
					}	
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsListLeaf(const Session& maike,DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("Leaf targets\n")
		.write("============\n");
	maike.targetsProcess(TargetsListLeaf(wb));
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
				auto ptr=dynamic_cast<const TargetPlaceholder*>(&target);
				if(ptr!=nullptr && ptr->relation()!=Dependency::Relation::FILE)
					{r_wb.write(" * ").write(target.nameGet()).write("\n");}
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsListExternal(const Session& maike,DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("External dependencies\n")
		.write("=====================\n");
	maike.targetsProcess(TargetsListExternal(wb));
	}

size_t Maike::targetsCountGet(const Session& maike)
	{
	return maike.targetsCountGet();
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
	std::vector<uint8_t> visited(id_range.second-id_range.first,0);
	toposort(Dependency(target_first),dependency_list,full,visited,id_range.first);
	}

static void buildBranch(Target& target,const char* target_dir
	,const Twins<size_t>& id_range)
	{
	std::vector<Dependency> dependency_list_in;
	toposort(target,dependency_list_in,id_range,1);

	auto deps_begin=dependency_list_in.data();
	Twins<Dependency*> deps(deps_begin,deps_begin + dependency_list_in.size());

	while(deps.first!=deps.second)
		{
		auto target=deps.first->target();
		if(target!=nullptr)
			{
			std::vector<Dependency> dependency_list;
			toposort(*target,dependency_list,id_range,0);

			std::vector<Dependency> dependency_list_full;
			toposort(*target,dependency_list_full,id_range,1);

			Twins<const Dependency*> deps_rel(dependency_list.data()
				,dependency_list.data() + dependency_list.size());

			Twins<const Dependency*> deps_rel_full(dependency_list_full.data()
				,dependency_list_full.data() + dependency_list_full.size());

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

//	For documentation purpose, dump the target database before
//	compiling anything.
//FIXME: This only writes one record (the current target), and
//not its dependencies. We must call toposort first.
	ResourceObjectJansson obj(ResourceObject::Type::ARRAY);
	targetDump(maike,obj,target_name);
	FileUtils::mkdir(maike.targetDirectoryGet());
	obj.write(FileOut(dircat(maike.targetDirectoryGet(),"__targets.json").c_str()));

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
				auto x=target.dependenciesInverseGet();
				if(x.second - x.first==0)
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
//	If there are only two targets, they must be the project root
//	directory, and the configuration file. In that case, do not
//	compile it, since that would create an almost empty target
//	directory.
	if(maike.targetsCountGet()<=2)
		{
		exceptionRaise(ErrorMessage("I could not identify any targets "
			"in any of the input directories.",{}));
		}
//	For documentation purpose, dump the target database before
//	compiling anything.
	FileUtils::mkdir(maike.targetDirectoryGet());
	targetsDump(maike).write(FileOut(dircat(maike.targetDirectoryGet(),"__targets.json").c_str()));

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

void Maike::graphInvDump(const Session& maike,GraphEdgeWriter& writer
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
			auto dep=node_current->dependenciesInverseGet();
			while(dep.first!=dep.second)
				{
				auto name_dep=dep.first->nameGet();
				if(strcmp(name_dep,".")!=0)
					{
					writer.edgeWrite(name_dep,name_target
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
	auto target_obj=db.createObject();
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
				auto target_obj=r_db.createObject();
				target.dump(target_obj);
				r_db.objectAppend(std::move(target_obj));
				return 0;
				}
		private:
			ResourceObject& r_db;
		};
	}

ResourceObject Maike::targetsDump(const Session& maike)
	{
	ResourceObjectJansson db(ResourceObject::Type::ARRAY);
	maike.targetsProcess(TargetDumpJSON(db));
	return std::move(db);
	}



void Maike::targetsDumpTSVHeader(DataSink& sink)
	{
	WriteBuffer wb(sink);
	wb.write("id\t")
		.write("name\t")
		.write("source_name\t")
		.write("dependency_inv_count\t")
		.write("dependency_count\t")
		.write("compilation_time\t")
		.write("line_count\n");
	}


static void targetDumpTSV(const Target& target,WriteBuffer& wb)
	{
	auto deps=target.dependencies();
	auto deps_inv=target.dependenciesInverseGet();
	wb.write(target.idGet()).write("\t")
		.write(target.nameGet()).write("\t")
		.write(target.sourceNameGet()).write("\t")
		.write(static_cast<size_t>(deps_inv.second-deps_inv.first)).write("\t")
		.write(static_cast<size_t>(deps.second-deps.first)).write("\t")
		.write(target.compilationTimeGet()).write("\t")
		.write(target.lineCountGet()).write("\n");
	}

namespace
	{
	class TargetDumpTSV:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetDumpTSV(WriteBuffer& wb):r_wb(wb)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				::targetDumpTSV(target,r_wb);
				return 0;
				}
		private:
			WriteBuffer& r_wb;
		};
	}

void Maike::targetsDumpTSV(const Session& maike,DataSink& sink)
	{
	WriteBuffer wb(sink);
	maike.targetsProcess(TargetDumpTSV(wb));
	}

void Maike::targetDumpTSV(const Session& maike,DataSink& sink,const char* target_name)
	{
	WriteBuffer wb(sink);
	::targetDumpTSV(maike.target(target_name),wb);
	}



static void deleteBranch(Target& target,const char* target_dir
	,const Twins<size_t>& id_range)
	{
	std::vector<Dependency> dependency_list_full;
	toposort(target,dependency_list_full,id_range,1);

	auto deps_begin=dependency_list_full.data();
	Twins<Dependency*> deps(deps_begin,deps_begin + dependency_list_full.size());

	while(deps.first!=deps.second)
		{
		--deps.second;
		auto target=deps.second->target();
		if(target!=nullptr)
			{
			FileUtils::remove( dircat(target_dir,target->nameGet() ).c_str());
			}
		}
	}

namespace
	{
	class TargetsDelete:public DependencyGraph::TargetProcessor
		{
		public:
			TargetsDelete(const Session& maike):
				r_target_dir(maike.targetDirectoryGet())
				,id_range(maike.targetIdRangeGet())
				{}

			int operator()(DependencyGraph& graph,Target& target)
				{
				auto x=target.dependenciesInverseGet();
				if(x.second - x.first==0)
					{deleteBranch(target,r_target_dir,id_range);}
				return 0;
				}
		private:
			const char* r_target_dir;
			Twins<size_t> id_range;
		};
	}

void Maike::clean(Session& maike)
	{
	maike.targetsProcess(TargetsDelete(maike));
	}

void Maike::clean(Session& maike,const char* target_name)
	{
	auto target_dir=maike.targetDirectoryGet();
	FileUtils::remove(dircat(target_dir,target_name).c_str());
	}


namespace
	{
	class TargetsMakeSet:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsMakeSet(const char* target_dir,std::set<Stringkey>& target_list):
				r_target_dir(target_dir),r_target_list(target_list)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				r_target_list.insert(Stringkey(dircat(r_target_dir,target.nameGet()).c_str()));
				return 0;
				}

		private:
			const char* r_target_dir;
			std::set<Stringkey>& r_target_list;
		};
	}

void Maike::removeOrphans(const Session& maike)
	{
	auto target_dir=maike.targetDirectoryGet();
	std::set<Stringkey> keeplist;
	maike.targetsProcess(TargetsMakeSet(target_dir,keeplist));
	FileUtils::removeTree(target_dir,keeplist);
	}
