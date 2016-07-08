//@	{"targets":[{"name":"targetpythonloader.o","type":"object"}]}

#include "targetpythonloader.hpp"
#include "readbuffer.hpp"
#include "filein.hpp"
#include "resourceobject.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "expressionevaluator.hpp"
#include "target_factorydelegator.hpp"
#include "dependencygraph.hpp"
#include "target.hpp"

using namespace Maike;

TargetPythonLoader::TargetPythonLoader(const TargetPythonInterpreter& intpret)
	:r_intpret(intpret)
	{}


namespace
	{
	class TagFilter final:public DataSource
		{
		public:
			TagFilter(DataSource& source):m_reader(source)
				,m_state(State::NEWLINE)
				{}

			TagFilter(DataSource&& source):TagFilter(source)
				{}

			size_t read(void* buffer,size_t length);

			const char* nameGet() const noexcept
				{return m_reader.nameGet();}

		private:
			ReadBuffer m_reader;
			enum class State:int{NEWLINE,COMMENT_0,JUNK,DATA};
			State m_state;

			void destroy()
				{delete this;}
		};
	}

size_t TagFilter::read(void* buffer,size_t length)
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
				if(!(ch_in<=' '))
					{
					switch(ch_in)
						{
						case '#':
							state=State::COMMENT_0;
							break;
						default:
							state=State::JUNK;
						}
					}
				break;


			case State::COMMENT_0:
				switch(ch_in)
					{
					case '@':
						state=State::DATA;
						break;
					default:
						state=State::JUNK;
					}
				break;

			case State::DATA:
				switch(ch_in)
					{
					case '\n':
					case '\r':
						*buffer_out='\n';
						++buffer_out;
						++n_read;
						state=State::NEWLINE;
						break;
					default:
						*buffer_out=ch_in;
						++buffer_out;
						++n_read;
					}
				break;

			case State::JUNK:
				switch(ch_in)
					{
					case '\n':
					case '\r':
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
	class TargetCreateCallback:public Target_FactoryDelegator::Callback
		{
		public:
			explicit TargetCreateCallback(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph):
				r_name_src(name_src),r_in_dir(in_dir),r_spider(spider),r_graph(graph)
				{}

			void operator()(const Target_FactoryDelegator& delegator
				,Handle<Target>&& target)
				{
				r_graph.targetRegister(std::move(target));
				}

		private:
			const char* r_name_src;
			const char* r_in_dir;
			Spider& r_spider;
			DependencyGraph& r_graph;
		};
	}


void TargetPythonLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	std::string name_full(in_dir);
	name_full+='/';
	name_full+=name_src;

	FileIn source(name_src);
	ResourceObject rc{TagFilter(source)};
	factory.targetsCreate(rc,name_src,in_dir
		,TargetCreateCallback(name_src,in_dir,spider,graph));
	}
