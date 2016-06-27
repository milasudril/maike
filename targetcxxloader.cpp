//@	{"targets":[{"name":"targetcxxloader.o","type":"object"}]}

#include "targetcxxloader.hpp"
#include "filein.hpp"
#include "targetcxxpptokenizer.hpp"
#include "spider.hpp"
#include <cstdio>

using namespace Maike;

namespace
	{
	class TagFilter:public DataSource
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
			enum class State:int{NEWLINE,COMMENT_0,JUNK,COMMENT_1,DATA};
			State m_state;
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
						case '/':
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
					case '/':
						state=State::COMMENT_1;
						break;
					default:
						state=State::JUNK;
					}
				break;

			case State::COMMENT_1:
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



static void includesGet(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph)
	{
	std::string name_full(in_dir);
	name_full+='/';
	name_full+=name_src;

	FileIn file_reader(name_full.c_str());
	TargetCxxPPTokenizer cpptok(file_reader);
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
					//TODO Implement this
						break;
					case TargetCxxPPTokenizer::Token::Type::STRING:
						printf("    Got %s\n",tok_in.value.c_str());
						spider.scanFile(tok_in.value.c_str(),in_dir);
						break;
					default:
					//CHECK Is this a legal case?
						break;
					}
				break;
			}
		}
	}

void TargetCxxLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph) const
	{
	printf("Loading targets from %s/%s\n",in_dir,name_src);

	std::string name_full(in_dir);
	name_full+='/';
	name_full+=name_src;

	FileIn source(name_full.c_str());
	TagFilter filter(source);
	ReadBuffer filter_reader(filter);
	while(!filter_reader.eof())
		{putchar(filter_reader.byteRead());}


	includesGet(name_src,in_dir,spider,graph);
	}
