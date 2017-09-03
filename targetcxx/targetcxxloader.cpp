//@	{"targets":[{"name":"targetcxxloader.o","type":"object"}]}

#include "targetcxxloader.hpp"
#include "targetcxxpptokenizer.hpp"
#include "targetcxx.hpp"
#include "../target_factorydelegator.hpp"
#include "../filein.hpp"
#include "../resourceobject.hpp"
#include "../dependency.hpp"
#include "../pathutils.hpp"
#include "../writebuffer.hpp"
#include "../stdstream.hpp"
#include "../dependencybuffer.hpp"

using namespace Maike;

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
			enum class State:int{NEWLINE,COMMENT_0,COMMENT_1,DATA,CODE};
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
					*buffer_out='\n';
					++buffer_out;
					++n_read;
					++m_lines;
					}
				if(!(ch_in<=' '))
					{
					switch(ch_in)
						{
						case '/':
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
					case '/':
						state=State::COMMENT_1;
						break;
					case '\r':
						break;
					case '\n':
						state=State::NEWLINE;
						*buffer_out='\n';
						++buffer_out;
						++n_read;
						++m_lines;
						break;
					default:
						state=State::CODE;
					}
				break;

			case State::COMMENT_1:
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
						state=State::NEWLINE;
						++m_lines;
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
						*buffer_out='\n';
						++buffer_out;
						++n_read;
						state=State::NEWLINE;
						break;
					}
				break;
			}
		}
	m_state=state;
	return n_read;
	}



TargetCxxLoader::TargetCxxLoader(const TargetCxxOptions& options
	,const TargetCxxCompiler& compiler):
	r_options(options),r_compiler(compiler)
	{}

Handle<Target> TargetCxxLoader::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root,size_t id,size_t line_count) const
	{return Handle<TargetCxx>( TargetCxx::create(obj,r_compiler,name_src,in_dir,root,id,line_count) );}


void TargetCxxLoader::dependenciesExtraGet(const char* name_src,const char* in_dir
	,const char* root,ResourceObject::Reader rc_reader,DependencyBuffer& deps_out) const
	{
	FileIn file(name_src);
	TagExtractor extractor(file);
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

void TargetCxxLoader::dependenciesGet(const char* name_src,const char* in_dir
	,const char* root,ResourceObject::Reader
	,DependencyBuffer& deps) const
	{
	FileIn file(name_src);
	WriteBuffer wb(StdStream::error());
	TargetCxxPPTokenizer cpptok(file);
	TargetCxxPPTokenizer::Token tok_in;
 	enum class Mode:uint8_t{NORMAL,INCLUDE};
	auto mode=Mode::NORMAL;
	while(cpptok.read(tok_in))
		{
		switch(mode)
			{
			case Mode::NORMAL:
				switch(tok_in.type)
					{
					case TargetCxxPPTokenizer::Token::Type::DIRECTIVE:
						if(tok_in.value=="include")
							{mode=Mode::INCLUDE;}
						break;
					default:
						break;
					}
				break;

			case Mode::INCLUDE:
				switch(tok_in.type)
					{
					case TargetCxxPPTokenizer::Token::Type::SYSINCLUDE:
					//TODO Implement this (Wand-style "old target lookup")
						break;
					case TargetCxxPPTokenizer::Token::Type::STRING:
						{
						auto name_dep_full=dircat(in_dir,tok_in.value);
						deps.append( Dependency(name_dep_full.c_str(),root,Dependency::Relation::INCLUDE) );
						}
						break;
					default:
						break;
					}
				mode=Mode::NORMAL;
				break;
			}
		}
	}

void TargetCxxLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	FileIn source(name_src);
	TagExtractor extractor(source);
	factory.targetsCreate(extractor,name_src,in_dir,*this,spider,graph);
	}
