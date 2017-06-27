//@	{"targets":[{"name":"targetxmlloader.o","type":"object"}]}

#include "targetxmlloader.hpp"
#include "targetxml.hpp"
#include "../readbuffer.hpp"
#include "../resourceobject.hpp"
#include "../exceptionhandler.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../target_factorydelegator.hpp"
#include "../fileutils.hpp"
#include "../pipe.hpp"
#include "../writebuffer.hpp"
#include "../stdstream.hpp"
#include "../parametersetmapfixed.hpp"
#include "../thread.hpp"

using namespace Maike;


void TargetXMLLoader::configClear()
	{
	m_filter=Command();
	}

TargetXMLLoader& TargetXMLLoader::configAppendDefault()
	{
	configClear();
	m_filter.nameSet("xsltproc").argumentAppend("{depfile}")
		.argumentAppend("{source}");
	return *this;
	}

TargetXMLLoader& TargetXMLLoader::configAppend(const ResourceObject& config)
	{
	if(config.objectExists("filter"))
		{m_filter=Command(config.objectGet("filter"));}
	return *this;
	}

void TargetXMLLoader::configDump(ResourceObject& config) const
	{
	auto filter=config.createObject();
	m_filter.configDump(filter);
	config.objectSet("filter",std::move(filter));
	}

Handle<Target> TargetXMLLoader::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root
	,size_t id,size_t line_count) const
	{
	return Handle<TargetXML>( TargetXML::create(obj,name_src,in_dir,root
		,id,line_count) );
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
					{wb.write("Error: ").write(message.messageGet()).write("\n");}
				}

		private:
			DataSource* r_src;
		};
	}

static Pipe run(const Command& cmd,const char* depfile,const char* source)
	{
	ParameterSetMapFixed<Stringkey("depfile"),Stringkey("source")> params;
	params.get<Stringkey("depfile")>().push_back(std::string(depfile));
	params.get<Stringkey("source")>().push_back(std::string(source));
	const ParameterSet* params_tot[]={&params};
	return cmd.execute(Pipe::REDIRECT_STDERR|Pipe::REDIRECT_STDOUT|Pipe::ECHO_OFF
		,Twins<const ParameterSet* const*>{params_tot,params_tot + 1});
	}

namespace
	{
	class TagExtractor final:public Target_FactoryDelegator::TagExtractor
		{
		public:
			explicit TagExtractor(const Command& cmd,const char* depfile,const char* source):
				 m_pipe(run(cmd,depfile,source))
				,m_src(source)
				,m_stderr(m_pipe.stderrCapture())
				,m_stderr_thread( ReadCallback{m_stderr.get()} )
				,m_stdout(m_pipe.stdoutCapture())
				{}

			size_t read(void* buffer,size_t length);

			const char* nameGet() const noexcept
				{return m_src.c_str();}

			size_t linesCountGet() const noexcept
				{return 1;}

			int exitStatusGet() noexcept
				{return m_pipe.exitStatusGet();}

		private:
			Pipe m_pipe;
			std::string m_src;
			Handle<DataSource> m_stderr;
			Thread<ReadCallback> m_stderr_thread;
			Handle<DataSource> m_stdout;
			void destroy()
				{delete this;}
		};
	}

size_t TagExtractor::read(void* buffer,size_t length)
	{
	return m_stdout->read(buffer,length);
	}

void TargetXMLLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	auto depfile=std::string(name_src);
	depfile+=".deps";
	if(!FileUtils::exists(depfile.c_str()))
		{return;}

	TagExtractor extractor(m_filter,depfile.c_str(),name_src);
	factory.targetsCreate(extractor,name_src,in_dir,*this,spider,graph);
	auto ret=extractor.exitStatusGet();
	if(ret!=0)
		{exceptionRaise(ErrorMessage("#0;: Failed to load any target definition.",{name_src}));}
	}
