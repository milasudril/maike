//@	{"targets":[{"name":"targetxmlloader.o","type":"object"}]}

#include "targetxmlloader.hpp"
#include "../readbuffer.hpp"
#include "../filein.hpp"
#include "../resourceobject.hpp"
#include "../exceptionhandler.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../target_factorydelegator.hpp"
#include "../dependencygraph.hpp"
#include "../target.hpp"
#include "../fileutils.hpp"

using namespace Maike;

TargetXMLLoader::TargetXMLLoader(const ParameterSetDumpable& sysvars):r_sysvars(sysvars)
	{}

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
	{return *this;}

void TargetXMLLoader::configDump(ResourceObject& config) const
	{}


namespace
	{
	class TagExtractor final:public Target_FactoryDelegator::TagExtractor
		{
		public:
			TagExtractor(DataSource& source):m_reader(source)
				,m_state(State::NEWLINE),m_lines(0)
				{}

			size_t read(void* buffer,size_t length);

			const char* nameGet() const noexcept
				{return m_reader.nameGet();}

			size_t linesCountGet() const noexcept
				{return m_lines;}

		private:
			ReadBuffer m_reader;
			enum class State:int{NEWLINE,COMMENT_0,CODE,DATA};
			State m_state;
			size_t m_lines;

			void destroy()
				{delete this;}
		};
	}

size_t TagExtractor::read(void* buffer,size_t length)
	{
	auto buffer_out=reinterpret_cast<uint8_t*>(buffer);
	size_t n_read=0;
	auto state=m_state;
	while(n_read!=length && !m_reader.eof())
		{
		auto ch_in=m_reader.byteRead();
		switch(state)
			{
			case State::NEWLINE:
				if(ch_in=='\n')
					{++m_lines;}
				if(!(ch_in<=' '))
					{
					switch(ch_in)
						{
						case '#':
							state=State::COMMENT_0;
							break;
						default:
							state=State::CODE;
						}
					}
				break;


			case State::COMMENT_0:
				switch(ch_in)
					{
					case '@':
						state=State::DATA;
						break;
					case '\r':
						break;
					case '\n':
						state=State::NEWLINE;
						++m_lines;
						break;
					default:
						state=State::CODE;
					}
				break;

			case State::DATA:
				switch(ch_in)
					{
					case '\r':
						break;
					case '\n':
						*buffer_out='\n';
						++buffer_out;
						++n_read;
						++m_lines;
						state=State::NEWLINE;
						break;
					default:
						*buffer_out=ch_in;
						++buffer_out;
						++n_read;
					}
				break;

			case State::CODE:
				switch(ch_in)
					{
					case '\r':
						break;
					case '\n':
						++m_lines;
						state=State::NEWLINE;
						break;
					}
				break;
			}
		}
	m_state=state;
	return n_read;
	}

namespace
	{
	class DependencyCollector:public Target_FactoryDelegator::DependencyCollector
		{
		public:
			bool operator()(const Target_FactoryDelegator& delegator,Dependency& dep_primary
				,ResourceObject::Reader rc_reader)
				{return 0;}
		};
	}

void TargetXMLLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	auto depfile=std::string(name_src);
	depfile+=".deps";
	if(!FileUtils::exists(depfile.c_str()))
		{return;}
	printf("%s\n",name_src);
/*	FileIn source(name_src);
	factory.targetsCreate(TagExtractor(source),name_src,in_dir
		,DependencyCollector(),graph);*/
	}
