//@	{"targets":[{"name":"targetxml.o","type":"object"}]}

#include "targetxml.hpp"
#include "../resourceobject.hpp"
#include "../fileutils.hpp"
#include "../pipe.hpp"
#include "../writebuffer.hpp"
#include "../readbuffer.hpp"
#include "../stdstream.hpp"
#include "../errormessage.hpp"
#include "../thread.hpp"
#include <cstring>

using namespace Maike;

TargetXML::TargetXML(const ResourceObject& obj
	,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,m_cmd(obj.objectGet("compiler")),m_root(root),m_status(0),m_static(1)
	{
	dependencyAdd(Dependency(m_cmd.nameGet(),Dependency::Relation::TOOL));
	}

void TargetXML::dumpDetails(ResourceObject& target) const
	{
	auto cmd=target.createObject();
	m_cmd.configDump(cmd);
	target.objectSet("compiler",std::move(cmd));
	}

bool TargetXML::upToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir) const
	{
	auto name_out=dircat(target_dir,nameGet());
	
	if(FileUtils::newer(sourceNameGet(),name_out.c_str()))
		{return 0;}

	if(dependency_list.first==dependency_list.second)
		{return m_status;}


	auto up_to_date=[&name_out](const char* name,Dependency::Relation rel)
		{return !FileUtils::newer(name,name_out.c_str());};

	if(!dependenciesProcess(target_dir,dependency_list,USE_ALL,up_to_date))
		{return 0;}

	return m_status==0;
	}


namespace
	{
	class ReadCallback
		{
		public:
			explicit ReadCallback(DataSource* src):r_src(src)
				{}

			void operator()()
				{
				WriteBuffer wb(StdStream::error());
				try
					{
					ReadBuffer rb(*r_src);
					while(!rb.eof())
						{wb.write(rb.byteRead());}
					}
				catch(const ErrorMessage& message)
					{wb.write("Error: ").write(message.messageGet());}
				}

		private:
			DataSource* r_src;
		};
	}

void TargetXML::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	std::vector<const char*> m_deps;
	m_cmd.argumentsProcess([&m_deps](const char* str)
		{m_deps.push_back(str);});

	auto name_out=dircat(target_dir,nameGet());
	m_deps.push_back(name_out.c_str());
	m_deps.push_back(sourceNameGet());
	dependenciesProcess(target_dir,dependency_list,USE_ALL
		,[&m_deps](const char* name,Dependency::Relation rel)
			{
			m_deps.push_back(name);
			return true;
			});

	Pipe pipe(m_cmd.nameGet()
		,{m_deps.data(),m_deps.data() + m_deps.size()},Pipe::REDIRECT_STDERR);
	auto standard_error=pipe.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	}

Maike::TargetXML::~TargetXML() noexcept
	{
	}


TargetXML* TargetXML::create(const ResourceObject& obj,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetXML(obj,name_src,in_dir,root,id,line_count);}

void TargetXML::destroy() noexcept
	{delete this;}
