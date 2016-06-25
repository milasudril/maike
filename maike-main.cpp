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
#include "targetdirectorycompiler.hpp"
#include "errormessage.hpp"

class TargetBuilder:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		explicit TargetBuilder(Maike::Invoker&& invoker):r_invoker(invoker)
			{}

		void operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			if(target_current.childCounterGet()==0)
				{Maike::buildBranch(target_current,r_invoker,graph.targetCounterGet());}
			}

	private:
		Maike::Invoker& r_invoker;
	};

int main(int argc,char** args)
	{
	try
		{
	//	Setup stuff
		std::map<Maike::Stringkey,const Maike::TargetLoader*> loaders;

		Maike::TargetDirectoryCompiler dircompiler;
		dircompiler.directoryTargetsSet("__targets");
		Maike::TargetDirectoryLoader dirloader(dircompiler);
		dirloader.pathRefuse(Maike::Stringkey(".git"))
			.pathRefuse(Maike::Stringkey("__targets"));
		loaders[Maike::Stringkey(".")]=&dirloader;

	//	Collect targtes
		Maike::DependencyGraphDefault targets;
		Maike::SpiderDefault spider(loaders,targets);
		spider.scanFile(".","").run();

	//	Build all targets
		targets.targetsPatch().targetsProcess(TargetBuilder{Maike::InvokerReal()});
		}
	catch(const Maike::ErrorMessage& msg)
		{
		printf("Error: %s",msg.messageGet());
		}
	catch(const char* msg)
		{printf("Error: %s",msg);}
	return 0;
	}
