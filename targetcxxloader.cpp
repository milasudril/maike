//@	{"targets":[{"name":"targetcxxloader.o","type":"object"}]}

#include "targetcxxloader.hpp"
#include "filein.hpp"
#include "targetcxxpptokenizer.hpp"
#include "spider.hpp"
#include "resourceobject.hpp"
#include "handle.hpp"
#include "target_factorydelegator.hpp"
#include "target.hpp"
#include "dependency.hpp"
#include "dependencygraph.hpp"

using namespace Maike;

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
			enum class State:int{NEWLINE,COMMENT_0,JUNK,COMMENT_1,DATA};
			State m_state;

			void destroy()
				{delete this;}
		};
	}

TargetCxxLoader::TargetCxxLoader(const TargetCxxOptions& options):
	r_options(options)
	{}

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
	,Spider& spider,DependencyGraph& graph,Target& target)
	{
	std::string name_full(in_dir);
	name_full+='/';
	name_full+=name_src;

	FileIn file_reader(name_src);
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
						{
						std::string name_dep_full(in_dir);
						name_dep_full+='/';
						name_dep_full+=tok_in.value;
						target.dependencyAdd(Dependency(name_dep_full.c_str()
							,Dependency::Relation::INTERNAL));
						spider.scanFile(name_dep_full.c_str(),in_dir);
						FileIn file(name_dep_full.c_str());
						ResourceObject obj{TagFilter(file)};
						if(obj.objectExists("dependencies_extra"))
							{
						//	Add dependency to the current target. This connects
						//	The the include file with the corresponding
						//	implementation file.
							auto deps=obj.objectGet("dependencies_extra");
							auto N=deps.objectCountGet();
							for(decltype(N) k=0;k<N;++k)
								{
								auto dep=deps.objectGet(k);
								auto ref=static_cast<const char*>( dep.objectGet("ref") );
								std::string ref_full(in_dir);
								ref_full+='/';
								ref_full+=ref;
							//TODO What if there was more than one target in the file we came from...
								if(ref_full!=target.nameGet())
									{target.dependencyAdd(Dependency(deps.objectGet(k),in_dir));}
								}
							}
						}
						break;
					default:
					//CHECK Is this a legal case?
						break;
					}
				mode=Mode::NORMAL;
				break;
			}
		}
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
				includesGet(r_name_src,r_in_dir,r_spider,r_graph,*target.get());
				r_graph.targetRegister(std::move(target));
				}

		private:
			const char* r_name_src;
			const char* r_in_dir;
			Spider& r_spider;
			DependencyGraph& r_graph;
		};
	}

void TargetCxxLoader::targetsLoad(const char* name_src,const char* in_dir
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
