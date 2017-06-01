//@	{"targets":[{"name":"targetcxxloader.o","type":"object"}]}

#include "targetcxxloader.hpp"
#include "targetcxxpptokenizer.hpp"
#include "../target_factorydelegator.hpp"
#include "../filein.hpp"
#include "../spider.hpp"
#include "../resourceobject.hpp"
#include "../handle.hpp"
#include "../target.hpp"
#include "../dependency.hpp"
#include "../dependencygraph.hpp"
#include "../pathutils.hpp"
#include "../writebuffer.hpp"
#include "../stdstream.hpp"
#include <cstring>
#include <vector>

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
					{++m_lines;}
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
						state=State::NEWLINE;
						break;
					}
				break;
			}
		}
	m_state=state;
	return n_read;
	}



TargetCxxLoader::TargetCxxLoader(const TargetCxxOptions& options):
	r_options(options)
	{}

namespace
	{
	class DependencyCollector:public Target_FactoryDelegator::DependencyCollector
		{
		public:
			DependencyCollector(const char* name_source,const char* in_dir,Spider& spider):
				 m_file_reader(name_source),m_cpptok(m_file_reader),r_in_dir(in_dir),r_spider(spider)
				,m_mode(Mode::NORMAL),m_target_include(0)
				{}

			bool operator()(const Target_FactoryDelegator&,Dependency& dep_primary
				,ResourceObject::Reader rc_reader);

			bool targetInclude() const noexcept
				{return m_target_include;}

		private:
			FileIn m_file_reader;
			TargetCxxPPTokenizer m_cpptok;
			std::vector<Dependency> m_deps_pending;
			const char* r_in_dir;
			Spider& r_spider;
 			enum class Mode:uint8_t{NORMAL,INCLUDE};
			Mode m_mode;
			bool m_target_include;
		};
	}

bool DependencyCollector::operator()(const Target_FactoryDelegator& delegator,Dependency& dep_primary
	,ResourceObject::Reader rc_reader)
	{
	WriteBuffer wb(StdStream::error());

	if(!m_deps_pending.empty())
		{
		dep_primary=std::move(m_deps_pending.back());
		m_deps_pending.pop_back();
		return 1;
		}
	TargetCxxPPTokenizer::Token tok_in;
	auto mode=m_mode;
	while(m_cpptok.read(tok_in))
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
						auto name_dep_full=dircat(r_in_dir,tok_in.value);
						dep_primary=Dependency(name_dep_full.c_str(),delegator.rootGet()
							,Dependency::Relation::INCLUDE);
						auto in_dir_include=dirname(name_dep_full);
						r_spider.scanFile(name_dep_full.c_str(),in_dir_include.c_str());

						FileIn file(name_dep_full.c_str());
						TagExtractor extractor(file);
						auto tags=rc_reader(extractor);
						if(tags.objectExists("dependencies_extra"))
							{
							auto deps=tags.objectGet("dependencies_extra");
							auto N=deps.objectCountGet();
							for(decltype(N) k=0;k<N;++k)
								{
								Dependency dep(deps.objectGet(k),in_dir_include.c_str()
									,delegator.rootGet());
								m_deps_pending.push_back(std::move(dep));
								}
							}
						m_mode=Mode::NORMAL;
						return 1;
						}
						break;
					default:
						{
						auto macro=m_cpptok.macroDecode(tok_in.value.c_str());
						if(macro.size()!=2)
							{break;}
						if(macro[0]=="MAIKE_TARGET")
							{
							wb.write(m_file_reader.nameGet()).write(": Warning: Using MAIKE_TARGET in this context is deprecated. "
								"Add `\"include_targets\":[\"").write(macro[1].c_str())
								.write("\"]`. See #44 for more information.\n");
							auto name_dep_full=dircat(r_in_dir,macro[1]);
							dep_primary=Dependency(name_dep_full.c_str(),delegator.rootGet()
								,Dependency::Relation::GENERATED);
							m_target_include=1;
							return 1;
							}
						}
						break;
					}
				mode=Mode::NORMAL;
				break;
			}
		}
	return 0;
	}

void TargetCxxLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	FileIn source(name_src);
	factory.targetsCreate(TagExtractor(source),name_src,in_dir
		,DependencyCollector(name_src,in_dir,spider),graph);
	}
