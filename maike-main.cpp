//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike.exe","type":"application"}]}]
//@		,{"targets":[{"name":"maike","type":"application"}]}
//@ ]

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "spiderdefault.hpp"
#include "stringkey.hpp"
#include "invokerreal.hpp"
#include "maike.hpp"
#include "targetdirectoryloader.hpp"
#include "errormessage.hpp"

#include <unistd.h>

class TargetBuilder:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		explicit TargetBuilder(Maike::Invoker&& invoker,const char* target_dir):
			r_invoker(invoker),m_target_dir(target_dir)
			{}

		void operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			if(target_current.childCounterGet()==0)
				{
				Maike::buildBranch(target_current,r_invoker
					,m_target_dir.c_str(),graph.targetCounterGet());
				}
			}

	private:
		Maike::Invoker& r_invoker;
		std::string m_target_dir;
	};

int main(int argc,char** args)
	{
	try
		{
	//	Setup stuff
		std::map<Maike::Stringkey,const Maike::TargetLoader*> loaders;

		Maike::TargetDirectoryLoader dirloader;
		dirloader.pathRefuse(Maike::Stringkey(".git"))
			.pathRefuse(Maike::Stringkey("__targets"));
		loaders[Maike::Stringkey(".")]=&dirloader;

	//	Collect targtes
		Maike::DependencyGraphDefault targets;
		Maike::SpiderDefault spider(loaders,targets);
		spider.scanFile(".","").run();

	//	Build all targets
		targets.targetsPatch().targetsProcess(TargetBuilder{Maike::InvokerReal(),"__targets"});
		}
	catch(const Maike::ErrorMessage& msg)
		{
		printf("Error: %s",msg.messageGet());
		}
	catch(const char* msg)
		{printf("Error: %s",msg);}
	return 0;
	}
