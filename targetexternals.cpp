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

TargetExternals::TargetExternals(const DependencyGraph& graph,size_t id):
	m_id(id)
	,m_dep(".",Dependency::Relation::INTERNAL)
	,m_compilation_time(std::numeric_limits<double>::quiet_NaN())
	,r_graph(graph)
	{
	}

void TargetExternals::compile(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir)
	{
	double t=0;
		{
		TimedScope timer(t);
		auto filename=dircat(target_dir,"externals.json");
		FileUtils::echo(m_data.c_str(),filename.c_str());
		}
	m_compilation_time+=t;
	}

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

static std::string data_reload(const char* filename)
	{
	std::string ret;
	try
		{
		ResourceObjectJansson src(FileIn{filename});
		src.write(StringWriter{ret});
		}
	catch(...)
		{}
	return std::move(ret);
	}

namespace
	{
	class TargetsExternalDump:public DependencyGraph::TargetProcessorConst
		{
		public:
			TargetsExternalDump(ResourceObject& libs,ResourceObject& tools):
				r_libs(libs),r_tools(tools)
				{}

			int operator()(const DependencyGraph&,const Target& target)
				{
				auto ptr=dynamic_cast<const TargetPlaceholder*>(&target);
				if(ptr!=nullptr)
					{
					switch(ptr->relation())
						{
						case Dependency::Relation::EXTERNAL:
							r_libs.objectAppend(r_libs.create(ptr->nameGet()));
							break;
						case Dependency::Relation::TOOL:
							r_tools.objectAppend(r_tools.create(ptr->nameGet()));
							break;
						default:
							break;
						}					
					}
				return 0;
				}

		private:
			ResourceObject& r_libs;
			ResourceObject& r_tools;
		};
	}

bool TargetExternals::upToDate(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir) const
	{
//	If there is content in m_data, we have been here before, but then we
//	must also have written the string content in compile. Since the graph
//	does not change during compilation, this target must be up to date.
	if(m_data.size()!=0) 
		{return 1;}

		{
		TimedScope timer(m_compilation_time);
		ResourceObjectJansson libs(ResourceObject::Type::ARRAY);
		ResourceObjectJansson tools(ResourceObject::Type::ARRAY);
		r_graph.targetsProcess(TargetsExternalDump{libs,tools});
		ResourceObjectJansson obj(ResourceObject::Type::OBJECT);
		obj.objectSet("libraries",std::move(libs))
			.objectSet("tools",std::move(tools));
		obj.write(StringWriter(m_data));
		}
	auto filename=dircat(target_dir,"externals.json");
	auto data=data_reload(filename.c_str());
	return m_data==data;
	}
