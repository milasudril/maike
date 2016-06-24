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
#include <vector>

class LeafCollector:public Maike::DependencyGraph::TargetProcessor
	{
	public:
		LeafCollector(std::vector<Maike::Target*>& leafs):r_leafs(leafs)
			{}

		void operator()(Maike::DependencyGraph& graph,Maike::Target& target_current)
			{
			if(target_current.childCounterGet()==0)
				{r_leafs.push_back(&target_current);}
			}

	private:
		std::vector<Maike::Target*>& r_leafs;
	};





int main(int argc,char** args)
	{
	Maike::DependencyGraphDefault graph;
	std::map<Maike::Stringkey,const Maike::TargetLoader*> loaders;
	Maike::SpiderDefault spider(loaders,graph);
	spider.scanFile(".").run();
	std::vector<Maike::Target*> leafs;
	graph.targetsPatch().targetsProcess(LeafCollector(leafs));

	Maike::InvokerReal exec_invoker;
	Maike::Twins<Maike::Target* const*> leafs_range
		{leafs.data(),leafs.data() + leafs.size()};
	Maike::buildAll(leafs_range,exec_invoker,graph.targetCounterGet());

	return 0;
	}
