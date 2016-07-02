//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike.exe","type":"application"}]}]
//@		,{"targets":[{"name":"maike","type":"application"}]}
//@	]

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "spiderdefault.hpp"
#include "stringkey.hpp"
#include "fileutils.hpp"
#include "maike.hpp"
#include "targetdirectoryloader.hpp"
#include "targetcxxloader.hpp"
#include "targetcxxcompiler.hpp"
#include "errormessage.hpp"
#include "dependency.hpp"
#include "sysvars.hpp"
#include "resourceobject.hpp"
#include "filein.hpp"
#include "expressionevaluatordefault.hpp"


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
				fprintf(m_dotfile,"\t\"%s\"->\"%s\";\n",name,deps.first->nameGet());
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
	//	Setup stuff
		auto config=configLoad("maikeconfig.json");

		Maike::ExpressionEvaluatorDefault evaluator;
		evaluator.sysvarsLoad();

		std::map<Maike::Stringkey,const Maike::TargetLoader*> loaders;

		Maike::TargetDirectoryLoader dirloader;
		dirloader.pathRefuse(Maike::Stringkey(".git"))
			.pathRefuse(Maike::Stringkey(".codelite"))
			.pathRefuse(Maike::Stringkey(".clang"))
			.pathRefuse(Maike::Stringkey("__targets"))
			.pathRefuse(Maike::Stringkey("test"));
		loaders[Maike::Stringkey(".")]=&dirloader;

		Maike::TargetCxxCompiler cxxcompiler(config.objectGet("cxxoptions"));
		Maike::TargetCxxLoader cxxloader(cxxcompiler);
		loaders[Maike::Stringkey(".hpp")]=&cxxloader;
		loaders[Maike::Stringkey(".cpp")]=&cxxloader;

	//	Collect targtes
		Maike::DependencyGraphDefault targets;
		Maike::SpiderDefault spider(loaders,evaluator,targets);
		spider.scanFile(".","").run();

	//	Build all targets
		targets.targetsPatch().targetsProcess(DepGraphExporter("dependencies.dot"));
		//	.targetsProcess(TargetBuilder{Maike::InvokerReal(),"__targets"});

		}
	catch(const Maike::ErrorMessage& msg)
		{
		printf("Error: %s",msg.messageGet());
		return -1;
		}
	catch(const char* msg)
		{
		printf("Error: %s",msg);
		return -1;
		}
	return 0;
	}
