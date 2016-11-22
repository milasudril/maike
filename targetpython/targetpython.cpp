//@	{"targets":[{"name":"targetpython.o","type":"object"}]}

#include "targetpython.hpp"
#include "targetpythoninterpreter.hpp"
#include "../resourceobject.hpp"

using namespace Maike;

TargetPython::TargetPython(const ResourceObject& obj
	,const TargetPythonInterpreter& intpret,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,r_intpret(intpret),m_root(root),m_status(1)
	{
	if(obj.objectExists("args"))
		{
		auto args=obj.objectGet("args");
		auto N=args.objectCountGet();
		for(decltype(N) k=0;k<N;++k)
			{m_args.push_back(std::string(static_cast<const char*>(args.objectGet(k))));}
		}

	dependencyAdd(Dependency("maikeconfig.json",root,Dependency::Relation::GENERATED));
	}

void TargetPython::dumpDetails(ResourceObject& target) const
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

void TargetPython::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	auto in_dir=dircat(m_root,inDirGet());
	auto cfg=dircat(target_dir,"maikeconfig.json");
	std::vector<const char*> args;
	args.push_back(target_dir);
	args.push_back(in_dir.c_str());
	args.push_back(cfg.c_str());
	auto i=m_args.data();
	auto i_end=i+m_args.size();
	while(i!=i_end)
		{
		args.push_back(i->c_str());
		++i;
		}
	m_status=r_intpret.run(sourceNameGet(),{args.data(),args.data() + args.size()});
	}

Maike::TargetPython::~TargetPython() noexcept
	{
	}


TargetPython* TargetPython::create(const ResourceObject& obj
	,const TargetPythonInterpreter& intpret,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetPython(obj,intpret,name_src,in_dir,root,id,line_count);}

void TargetPython::destroy() noexcept
	{delete this;}
