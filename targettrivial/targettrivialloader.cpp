//@	{"targets":[{"name":"targettrivialloader.o","type":"object"}]}

#include "targettrivialloader.hpp"
#include "targettrivial.hpp"
#include "../target_factorydelegator.hpp"
#include "../filein.hpp"
#include "../readbuffer.hpp"

using namespace Maike;

Handle<Target> TargetTrivialLoader::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root
	,size_t id,size_t line_count) const
	{
	return Handle<TargetTrivial>( TargetTrivial::create(obj,name_src,in_dir,root
		,id,line_count) );
	}

namespace
	{
	class TagExtractor final:public Target_FactoryDelegator::TagExtractor
		{
		public:
			TagExtractor(DataSource& source):m_reader(source),m_lines(0)
				{}

			size_t read(void* buffer,size_t length);

			const char* nameGet() const noexcept
				{return m_reader.nameGet();}

			size_t linesCountGet() const noexcept
				{return m_lines;}

		private:
			ReadBuffer m_reader;
			size_t m_lines;

			void destroy()
				{delete this;}
		};
	}

size_t TagExtractor::read(void* buffer,size_t length)
	{
	auto buffer_out=reinterpret_cast<uint8_t*>(buffer);
	size_t n_read=0;
	while(n_read!=length && !m_reader.eof())
		{
		auto ch_in=m_reader.byteRead();
		if(ch_in=='\n')
			{++m_lines;}
		*buffer_out=ch_in;
		++n_read;
		++buffer_out;
		}
	return n_read;
	}

void TargetTrivialLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	FileIn src(name_src);
	TagExtractor extractor(src);
	factory.targetsCreate(extractor,name_src,in_dir,*this,spider,graph);
	}
