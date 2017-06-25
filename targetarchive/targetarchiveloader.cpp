//@	{"targets":[{"name":"targetarchiveloader.o","type":"object"}]}

#include "targetarchiveloader.hpp"
#include "../readbuffer.hpp"
#include "../filein.hpp"
#include "../resourceobject.hpp"
#include "../exceptionhandler.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../target_factorydelegator.hpp"
#include "../dependencygraph.hpp"
#include "../target.hpp"
#include "../dependency.hpp"
#include "../dependencybuffer.hpp"
#include <vector>
#include <cstring>

using namespace Maike;

TargetArchiveLoader::TargetArchiveLoader()
	{}

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

void TargetArchiveLoader::dependenciesGet(const char* name_src,const char* in_dir
	,const char* root,ResourceObject::Reader rc_reader,DependencyBuffer& deps) const
	{
	FileIn src(name_src);
	auto rc=rc_reader(src);
	if(!rc.objectExists("contents"))
		{return;}
	
	auto contents=rc.objectGet("contents");
	if(contents.typeGet()!=ResourceObject::Type::ARRAY)
		{return;}

	auto N=contents.objectCountGet();

	for(decltype(N) n=0;n<N;++n)
		{
		auto obj=contents.objectGet(n);
	//TODO: The new system only requires an array of strings, no keys needed anymore
		auto file=static_cast<const char*>(obj.objectGet("file"));
		auto filename_full=dircat(in_dir,file);
		deps.append(Dependency(filename_full.c_str(),root,Dependency::Relation::MISC));
		}
	}

void TargetArchiveLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	FileIn source(name_src);
	TagExtractor extractor(source);
	factory.targetsCreate(extractor,name_src,in_dir,*this,spider,graph);
	}
