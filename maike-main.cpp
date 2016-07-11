//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike.exe","type":"application"}]}]
//@		,["1",{"targets":[{"name":"maike","type":"application"}]}]
//@	]

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "spiderdefault.hpp"
#include "stringkey.hpp"
#include "maike.hpp"

#include "targetdirectoryloader.hpp"

#include "targetcxxhook.hpp"
#include "targetpythonhook.hpp"

#include "target_factorydelegatordefault.hpp"

#include "errormessage.hpp"
#include "dependency.hpp"
#include "resourceobject.hpp"
#include "filein.hpp"
#include "expressionevaluatordefault.hpp"
#include "systemtargetinfo.hpp"
#include "options.hpp"

#include "variant.hpp"
#include "strerror.hpp"
#include "datasinkstd.hpp"
#include <cstdio>
#include <cstring>

class TargetBuilder:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		explicit TargetBuilder(const char* target_dir):m_target_dir(target_dir)
			{}

		int operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			if(target_current.childCounterGet()==0)
				{
				Maike::buildBranch(target_current,m_target_dir.c_str(),graph.idRangeGet());
				}
			return 0;
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

		int operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			auto name=target_current.nameGet();
			if(strcmp(name,".")!=0)
				{
				auto deps=target_current.dependencies();
				while(deps.first!=deps.second)
					{
					if(strcmp(deps.first->nameGet(),".")!=0)
						{
						const char* colorstring=deps.first->relationGet()==Maike::Dependency::Relation::IMPLEMENTATION?
							"red":"blue";
						fprintf(m_dotfile,"\t\"%s\"->\"%s\"[color=\"%s\"];\n",name
							,deps.first->nameGet(),colorstring);
						}
					++deps.first;
					}
				}
			return 0;
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

	//	1.1 Load Target system information
		Maike::SystemTargetInfo targetinfo(maikeconfig.objectGet("targetconfig"));
	//	targetinfo.dataDump(Maike::DataSinkStd::standard_output);


	//	1.2 Directory loader (responsible for scanning directories)
		Maike::TargetDirectoryLoader dirloader(maikeconfig.objectGet("directoryloader"));

	//	1.3 C++ loader and compiler
		Maike::Handle<Maike::TargetCxxHook> cxxhook(
			Maike::TargetCxxHook::create(maikeconfig.objectGet("cxxoptions")
				,targetinfo));
		auto& cxxfactory=cxxhook->factoryGet();
		auto& cxxloader=cxxhook->loaderGet();


	//	1.4 Python loader and interpreter
		Maike::Handle<Maike::TargetPythonHook> pythonhook(
			Maike::TargetPythonHook::create(maikeconfig.objectGet("pythonoptions")
				,targetinfo));
		auto& pythonfactory=pythonhook->factoryGet();
		auto& pythonloader=pythonhook->loaderGet();


	//	2. Collect targtes
		Maike::ExpressionEvaluatorDefault evaluator(targetinfo);

		Maike::IdGenerator<size_t> id_gen;
		Maike::Target_FactoryDelegatorDefault delegator(
			 static_cast<const char*>(targetinfo.variableGet(Maike::Stringkey("target_directory")))
			,evaluator
			,id_gen);
		delegator.factoryRegister(Maike::Stringkey(".py"),pythonfactory)
			.factoryRegister(Maike::Stringkey(".cpp"),cxxfactory)
			.factoryRegister(Maike::Stringkey(".hpp"),cxxfactory);

		Maike::DependencyGraphDefault targets(id_gen);
		Maike::SpiderDefault spider(delegator,targets);
		spider.loaderRegister(Maike::Stringkey(".cpp"),cxxloader)
			.loaderRegister(Maike::Stringkey(".hpp"),cxxloader)
			.loaderRegister(Maike::Stringkey(".py"),pythonloader)
			.loaderRegister(Maike::Stringkey("."),dirloader)
			.scanFile(".","").run();

	//	3. Build all targets
		targets.targetsPatch().targetsProcess(DepGraphExporter("dependencies.dot"))
			.targetsProcess(TargetBuilder{"__targets"});

		}
	catch(const Maike::ErrorMessage& msg)
		{
		fprintf(stderr,"%s\n",msg.messageGet());
		return -1;
		}
	catch(const char* msg)
		{
		fprintf(stderr,"%s",msg);
		return -1;
		}
	return 0;
	}
