//@	{
//@	 "targets":[{"name":"targetconfig.o","type":"object"}]
//@	}

#include "targetconfig.hpp"
#include "datasink.hpp"
#include "resourceobject.hpp"
#include "fileutils.hpp"
#include "pathutils.hpp"
#include "timedscope.hpp"
#include <limits>

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

TargetConfig::TargetConfig(const ResourceObject& config,size_t id):
	m_id(id)
	,m_dep(".",Dependency::Relation::INTERNAL)
	,m_compilation_time(std::numeric_limits<double>::quiet_NaN())
	{
	StringWriter writer(m_content);
	config.write(writer);
	}

void TargetConfig::compile(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	TimedScope scope(m_compilation_time);
	auto filename=dircat(target_dir,"maikeconfig.json");
	FileUtils::echo(m_content.c_str(),filename.c_str());
	}
