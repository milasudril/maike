//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike.exe","type":"application"}]}]
//@		,{"targets":[{"name":"maike","type":"application"}]}
//@	]

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "spiderdefault.hpp"
#include "stringkey.hpp"
#include "maike.hpp"

#include "targetdirectoryloader.hpp"

#include "targetcxxoptions.hpp"
#include "targetcxxloader.hpp"
#include "targetcxxcompiler.hpp"

#include "errormessage.hpp"
#include "dependency.hpp"
#include "resourceobject.hpp"
#include "filein.hpp"
#include "expressionevaluatordefault.hpp"
#include "systemtargetinfo.hpp"

#include "variant.hpp"
#include "strerror.hpp"
#include <cstdio>

class TargetBuilder:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		explicit TargetBuilder(const char* target_dir):m_target_dir(target_dir)
			{}

		void operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			if(target_current.childCounterGet()==0)
				{
				Maike::buildBranch(target_current,m_target_dir.c_str(),graph.targetCounterGet());
				}
			}

	private:
		std::string m_target_dir;
	};

class DepGraphExporter:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		explicit DepGraphExporter(const char* file_out)
			{
			m_dotfile=fopen(file_out,"w");
			if(m_dotfile==NULL)
				{
				throw Maike::ErrorMessage("It was not possible to create the dot file. #0;"
					,{static_cast<const char*>(Maike::strerror(errno))});
				}
			fprintf(m_dotfile,"digraph DependencyGraph\n\t{\n");
			}

		void operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			auto name=target_current.nameGet();
			auto deps=target_current.dependencies();
			while(deps.first!=deps.second)
				{
				const char* colorstring=deps.first->relationGet()==Maike::Dependency::Relation::IMPLEMENTATION?
					"red":"blue";
				fprintf(m_dotfile,"\t\"%s\"->\"%s\"[color=\"%s\"];\n",name
					,deps.first->nameGet(),colorstring);
				++deps.first;
				}
			}

		~DepGraphExporter()
			{
			fprintf(m_dotfile,"\t}\n");
			fclose(m_dotfile);
			}

	private:
		FILE* m_dotfile;
	};

static Maike::ResourceObject configLoad(const char* filename)
	{
	Maike::FileIn source(filename);
	return Maike::ResourceObject(source);
	}

int main(int argc,char** args)
	{
	try
		{
	//	1 Setup stuff
		auto maikeconfig=configLoad("maikeconfig.json");
	//	1.2 Target system information
		Maike::SystemTargetInfo targetinfo(maikeconfig.objectGet("targetconfig"));
		Maike::ExpressionEvaluatorDefault evaluator(targetinfo);

	//	1.3	Setup Loaders and compilers
		std::map<Maike::Stringkey,const Maike::TargetLoader*> loaders;

	//	1.3.1 Directory loader (responsible for scanning directories)
		Maike::TargetDirectoryLoader dirloader(maikeconfig.objectGet("directoryloader"));
		loaders[Maike::Stringkey(".")]=&dirloader;

	//	1.3.2 C++ loader and compiler
		Maike::TargetCxxOptions cxxoptions(maikeconfig.objectGet("cxxoptions"));
		Maike::TargetCxxCompiler cxxcompiler(cxxoptions,targetinfo);
		Maike::TargetCxxLoader cxxloader(cxxcompiler);
		loaders[Maike::Stringkey(".hpp")]=&cxxloader;
		loaders[Maike::Stringkey(".cpp")]=&cxxloader;

	//	2. Collect targtes
		Maike::DependencyGraphDefault targets;
		Maike::SpiderDefault spider(loaders,evaluator,targets);
		spider.scanFile(".","").run();

	//	3. Build all targets
		targets.targetsPatch().targetsProcess(DepGraphExporter("dependencies.dot"))
			.targetsProcess(TargetBuilder{"__targets"});

		}
	catch(const Maike::ErrorMessage& msg)
		{
		fprintf(stderr,"Error: %s",msg.messageGet());
		return -1;
		}
	catch(const char* msg)
		{
		fprintf(stderr,"Error: %s",msg);
		return -1;
		}
	return 0;
	}
