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

namespace
	{
	class DependencyCollector:public Target_FactoryDelegator::DependencyCollector
		{
		public:
			DependencyCollector(const char* name_src,const char* in_dir):
				r_name_src(name_src),r_in_dir(in_dir)
				{r_dep_ptr=m_deps_pending.data();}

			bool operator()(const Target_FactoryDelegator& delegator,Dependency& dep_primary
				,ResourceObject::Reader rc_reader);


		private:
			const char* r_name_src;
			const char* r_in_dir;
			std::vector<Dependency> m_deps_pending;
			const Dependency* r_dep_ptr;
		};
	}

/*static Dependency::Relation dependencyRelation(const char* str)
	{
	return strcmp(str,"target")?Dependency::Relation::FILE
		:Dependency::Relation::GENERATED;
	}*/

bool DependencyCollector::operator()(const Target_FactoryDelegator& delegator,Dependency& dep_primary
	,ResourceObject::Reader rc_reader)
	{
	if(r_dep_ptr!=m_deps_pending.data())
		{
		--r_dep_ptr;
		dep_primary=std::move(*r_dep_ptr);
		return 1;
		}
	if(m_deps_pending.size()!=0)
		{return 0;}
	
	FileIn src(r_name_src);
	auto rc=rc_reader(src);
	if(!rc.objectExists("contents"))
		{return 0;}
	
	auto contents=rc.objectGet("contents");
	if(contents.typeGet()!=ResourceObject::Type::ARRAY)
		{return 0;}

	auto N=contents.objectCountGet();

	for(decltype(N) n=0;n<N;++n)
		{
		auto obj=contents.objectGet(n);
	//	auto from=static_cast<const char*>(obj.objectGet("from"));
		auto file=static_cast<const char*>(obj.objectGet("file"));
		auto filename_full=dircat(r_in_dir,file);
		m_deps_pending.push_back(Dependency(filename_full.c_str(),delegator.rootGet()
			,Dependency::Relation::MISC));
		}
	r_dep_ptr=m_deps_pending.data() + m_deps_pending.size();
	if(r_dep_ptr!=m_deps_pending.data())
		{
		--r_dep_ptr;
		dep_primary=std::move(*r_dep_ptr);
		return 1;
		}
	return 0;
	}

void TargetArchiveLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	FileIn source(name_src);
	factory.targetsCreate(TagExtractor(source),name_src,in_dir
		,DependencyCollector(name_src,in_dir),graph);
	}
