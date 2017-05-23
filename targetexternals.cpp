//@	{
//@	 "targets":[{"name":"targetexternals.o","type":"object"}]
//@	}

#include "targetexternals.hpp"
#include "targetplaceholder.hpp"
#include "datasink.hpp"
#include "resourceobjectjansson.hpp"
#include "fileutils.hpp"
#include "pathutils.hpp"
#include "timedscope.hpp"
#include "filein.hpp"
#include "dependencygraph.hpp"
#include <limits>
#include <algorithm>

using namespace Maike;

namespace
	{
	class StringWriter:public DataSink
		{
		public:
			StringWriter(std::string& sink):r_sink(sink)
				{}
			
			size_t write(const void* buffer,size_t n)
				{
				auto ptr=reinterpret_cast<const char*>(buffer);
				auto ptr_ref=ptr;
				while(n!=0)
					{
					auto ch_in=*ptr;
					if(ch_in=='\0')
						{return static_cast<size_t>(ptr - ptr_ref);}
					r_sink+=ch_in;
					--n;
					++ptr;
					}
				return static_cast<size_t>(ptr - ptr_ref);
				}

			void destroy(){}

		private:
			std::string& r_sink;
		};
	}

TargetExternals::TargetExternals(const DependencyGraph& graph,size_t id):
	m_id(id)
	,m_dep(".",Dependency::Relation::INTERNAL)
	,m_compilation_time(std::numeric_limits<double>::quiet_NaN())
	,r_graph(graph)
	{
	}

namespace
	{

	class TargetsExternalDump:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsExternalDump(std::vector<std::string>& libs
				,std::vector<std::string>& tools):r_libs(libs),r_tools(tools)
				{}

			int operator()(const DependencyGraph& graph,const Target& target)
				{
				auto ptr=dynamic_cast<const TargetPlaceholder*>(&target);
				if(ptr!=nullptr)
					{
					switch(ptr->relation())
						{
						case Dependency::Relation::EXTERNAL:
							r_libs.push_back(ptr->nameGet());
							break;
						case Dependency::Relation::TOOL:
							r_tools.push_back(ptr->nameGet());
							break;
						default:
							break;
						}					
					}
				return 0;
				}

		private:
			std::vector<std::string>& r_libs;
			std::vector<std::string>& r_tools;
		};
	}

void TargetExternals::compile(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	TimedScope scope(m_compilation_time);
	auto filename=dircat(target_dir,"externals.json");
	
	std::vector<std::string> libs;
	std::vector<std::string> tools;
	r_graph.targetsProcess(TargetsExternalDump{libs,tools});
	std::string json_out(R"EOF({
	"libs":[)EOF");
	std::for_each(libs.begin(),libs.end()
		,[&json_out](const std::string& str)
			{
			json_out+='"';
			//TODO: Escape str
			json_out+=str;
			json_out+="\",";
			});
	json_out.pop_back();
	json_out+=R"EOF(]
	,"tools":[)EOF";
	std::for_each(tools.begin(),tools.end()
		,[&json_out](const std::string& str)
			{
			json_out+='"';
			//TODO: Escape str
			json_out+=str;
			json_out+="\",";
			});
	json_out.pop_back();
	json_out+=R"EOF(]
})EOF";
	FileUtils::echo(json_out.c_str(),filename.c_str());
	}

bool TargetExternals::upToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir) const
	{
	auto filename=dircat(target_dir,"externals.json");
	if(!FileUtils::exists(filename.c_str()))
		{return 0;}
	
	return 0;
	}
