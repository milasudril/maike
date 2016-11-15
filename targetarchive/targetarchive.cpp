//@	{"targets":[{"name":"targetarchive.o","type":"object"}]}

#include "targetarchive.hpp"
#include "targetarchivecompiler.hpp"
#include "../resourceobject.hpp"

using namespace Maike;

TargetArchive::TargetArchive(const ResourceObject& obj
	,const TargetArchiveCompiler& intpret,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,r_intpret(intpret),m_status(1)
	{
	if(obj.objectExists("args"))
		{
		auto args=obj.objectGet("args");
		auto N=args.objectCountGet();
		for(decltype(N) k=0;k<N;++k)
			{m_args.push_back(std::string(static_cast<const char*>(args.objectGet(k))));}
		}
	}

void TargetArchive::dumpDetails(ResourceObject& target) const
	{
	auto args=target.createArray();
	auto i=m_args.data();
	auto i_end=i+m_args.size();
	while(i!=i_end)
		{
		args.objectAppend(args.create(i->c_str()));
		++i;
		}
	target.objectSet("args",std::move(args));
	}

void TargetArchive::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	std::vector<const char*> args;
	args.push_back(target_dir);
	auto i=m_args.data();
	auto i_end=i+m_args.size();
	while(i!=i_end)
		{
		args.push_back(i->c_str());
		++i;
		}

	m_status=r_intpret.run(sourceNameGet(),{args.data(),args.data() + args.size()});
	}

Maike::TargetArchive::~TargetArchive() noexcept
	{
	}


TargetArchive* TargetArchive::create(const ResourceObject& obj
	,const TargetArchiveCompiler& intpret,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetArchive(obj,intpret,name_src,in_dir,root,id,line_count);}

void TargetArchive::destroy() noexcept
	{delete this;}
