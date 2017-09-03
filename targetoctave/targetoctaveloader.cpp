//@	{"targets":[{"name":"targetoctaveloader.o","type":"object"}]}

#include "targetoctaveloader.hpp"
#include "targetoctave.hpp"
#include "../readbuffer.hpp"
#include "../filein.hpp"
#include "../resourceobject.hpp"
#include "../target_factorydelegator.hpp"
#include "../dependencybuffer.hpp"

using namespace Maike;

TargetOctaveLoader::TargetOctaveLoader(const TargetOctaveInterpreter& intpret):
	r_intpret(intpret)
	{}

Handle<Target> TargetOctaveLoader::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root
	,size_t id,size_t line_count) const
	{
	return Handle<TargetOctave>( TargetOctave::create(obj,r_intpret,name_src,in_dir,root
		,id,line_count) );
	}

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
					{
					++m_lines;
					*buffer_out=ch_in;
					++n_read;
					++buffer_out;
					}
				if(!(ch_in<=' '))
					{
					switch(ch_in)
						{
						case '#':
						case '%':
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
						*buffer_out=ch_in;
						++buffer_out;
						++n_read;
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
						*buffer_out=ch_in;
						++n_read;
						++buffer_out;
						state=State::NEWLINE;
						break;
					}
				break;
			}
		}
	m_state=state;
	return n_read;
	}


void TargetOctaveLoader::dependenciesExtraGet(const char* name_src,const char* in_dir
	,const char* root,ResourceObject::Reader rc_reader
	,DependencyBuffer& deps_out) const
	{
	FileIn src(name_src);
	TagExtractor extractor(src);
	auto tags=rc_reader(extractor);
	if(tags.objectExists("dependencies_extra"))
		{
		auto deps=tags.objectGet("dependencies_extra");
		auto N=deps.objectCountGet();
		for(decltype(N) k=0;k<N;++k)
			{
			Dependency dep(deps.objectGet(k),in_dir,root);
			deps_out.append(std::move(dep));
			}
		}
	}

void TargetOctaveLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	FileIn source(name_src);
	TagExtractor extractor(source);
	factory.targetsCreate(extractor,name_src,in_dir,*this,spider,graph);
	}
