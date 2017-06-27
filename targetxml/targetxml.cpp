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
#include "../variant.hpp"
#include "../exceptionhandler.hpp"
#include <cstring>

using namespace Maike;

TargetXML::TargetXML(const ResourceObject& obj
	,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,m_cmd(obj.objectGet("compiler")),m_root(root)
	{
	dependencyAdd(Dependency(m_cmd.nameGet(),Dependency::Relation::TOOL));
	}

void TargetXML::dumpDetails(ResourceObject& target) const
	{
	auto cmd=target.createObject();
	m_cmd.configDump(cmd);
	target.objectSet("compiler",std::move(cmd));
	}

bool TargetXML::upToDate(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir) const
	{
	auto name_out=dircat(target_dir,nameGet());
	
	if(FileUtils::newer(sourceNameGet(),name_out.c_str()))
		{return 0;}

	auto up_to_date=[&name_out](const char* name,Dependency::Relation)
		{return !FileUtils::newer(name,name_out.c_str());};

	return dependenciesProcess(target_dir,dependencies(),USE_INCLUDE_EXTRA,up_to_date);
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

void TargetXML::compileImpl(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir)
	{
	std::vector<const char*> args;
	m_cmd.argumentsProcess([&args](const char* str)
		{args.push_back(str);});

	auto name_out=dircat(target_dir,nameGet());
	args.push_back(name_out.c_str());
	args.push_back(sourceNameGet());

	std::vector<std::string> deps;
	dependenciesProcess(target_dir,dependencies(),USE_INCLUDE_EXTRA
		,[&deps](const char* name,Dependency::Relation)
			{
			deps.push_back(std::string(name));
			return true;
			});

	std::for_each(deps.begin(),deps.end(),[&args](const std::string& str)
		{args.push_back(str.c_str());});

	Pipe pipe(m_cmd.nameGet()
		,{args.data(),args.data() + args.size()},Pipe::REDIRECT_STDERR);
	auto standard_error=pipe.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});

	auto ret=pipe.exitStatusGet();
	if(ret!=0)
		{
		exceptionRaise(ErrorMessage("#0;: Compilation failed with status #1;.",{sourceNameGet(),ret}));
		}
	}

Maike::TargetXML::~TargetXML() noexcept
	{
	}


TargetXML* TargetXML::create(const ResourceObject& obj,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetXML(obj,name_src,in_dir,root,id,line_count);}

void TargetXML::destroy() noexcept
	{delete this;}
