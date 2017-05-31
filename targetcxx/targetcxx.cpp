//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"
#include "targetcxxpkgconfig.hpp"
#include "targetcxxcompiler.hpp"
#include "../resourceobject.hpp"
#include "../stringkey.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../exceptionhandler.hpp"
#include "../fileutils.hpp"
#include "../pathutils.hpp"
#include "../writebuffer.hpp"
#include "../stdstream.hpp"
#include <algorithm>

using namespace Maike;

static TargetCxx::Type type(const char* name_src,const char* type)
	{
	auto key=Stringkey(type);
	if(key==Stringkey("object"))
		{return TargetCxx::Type::OBJECT;}

	if(key==Stringkey("include"))
		{return TargetCxx::Type::INCLUDE;}

	if(key==Stringkey("application"))
		{return TargetCxx::Type::APPLICATION;}

	if(key==Stringkey("lib_dynamic"))
		{return TargetCxx::Type::LIB_DYNAMIC;}

	if(key==Stringkey("lib_static"))
		{return TargetCxx::Type::LIB_STATIC;}

	if(key==Stringkey("include_lib"))
		{return TargetCxx::Type::INCLUDE_LIB;}

	exceptionRaise(ErrorMessage("#0;: Unknown target type #1;.",{name_src,type}));
	}

static const char* type(TargetCxx::Type type)
	{
	switch(type)
		{
		case TargetCxx::Type::OBJECT:
			return "object";
		case TargetCxx::Type::INCLUDE:
			return "include";
		case TargetCxx::Type::APPLICATION:
			return "application";
		case TargetCxx::Type::LIB_DYNAMIC:
			return "lib_dynamic";
		case TargetCxx::Type::LIB_STATIC:
			return "lib_static";
		case TargetCxx::Type::INCLUDE_LIB:
			return "include_lib";
		}
	return nullptr;
	}

void TargetCxx::pkgconfig(const ResourceObject& pkgconfig_libs)
	{
	auto cmd=(r_compiler.optionsGet()|m_options_extra).pkgconfigGet();
	auto N=pkgconfig_libs.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		r_compiler.pkgconfigAsk(static_cast<const char*>(pkgconfig_libs.objectGet(k)),sourceNameGet())
			.optionsPush(m_options_extra).dependenciesPush(*this);
		}
	}

TargetCxx::TargetCxx(const ResourceObject& obj,const TargetCxxCompiler& compiler
	,const char* name_src,const char* in_dir,const char* root,size_t id
	,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,r_compiler(compiler)
	{
	m_type=type(name_src,static_cast<const char*>( obj.objectGet("type") ));

	if(obj.objectExists("cxxoptions"))
		{m_options_extra=TargetCxxOptions(obj.objectGet("cxxoptions"));}
	if(obj.objectExists("pkgconfig_libs"))
		{pkgconfig(obj.objectGet("pkgconfig_libs"));}
	if(obj.objectExists("include_targets"))
		{
		auto itargets=obj.objectGet("include_targets");
		auto N=itargets.objectCountGet();
		for(decltype(N) k=0;k<N;++k)
			{
			auto dep_name=static_cast<const char*>(itargets.objectGet(k));
			Dependency dep( dircat(in_dir,dep_name).c_str(),root,Dependency::Relation::INCLUDE_GENERATED );
			dependencyAdd( std::move(dep) );
			}
		}
	}

void TargetCxx::dumpDetails(ResourceObject& target) const
	{
	target.objectSet("type",target.create(type(typeGet())));

	auto cxxoptions=target.createObject();
	m_options_extra.configDump(cxxoptions);
	target.objectSet("cxxoptions",std::move(cxxoptions));
	}

static constexpr int USE_EXTERNAL=1;
static constexpr int USE_IMPLEMENTATION=2;
static constexpr int USE_INCLUDE=4;
static constexpr int USE_INCLUDE_GENERATED=8;
static constexpr int USE_GENERATED=16;
static constexpr int USE_FILE=32;
static constexpr int USE_LEAF=64;
static constexpr int USE_INTERNAL=128;

template<class Proc>
static void dependenciesProcess(const char* target_dir,Twins<const Dependency*> deps
	,int use_flags,Proc&& proc)
	{
	while(deps.first!=deps.second)
		{
		const auto rel=deps.first->relationGet();
		if(rel==Dependency::Relation::EXTERNAL && (use_flags&USE_EXTERNAL) )
			{
			auto t=deps.first->target();
			proc(t->nameGet(),rel);
			}
		if(rel==Dependency::Relation::IMPLEMENTATION && (use_flags&USE_IMPLEMENTATION) )
			{
			auto name_full=dircat(target_dir,deps.first->target()->nameGet());
			proc(name_full.c_str(),rel);
			}
		if(rel==Dependency::Relation::INCLUDE && (use_flags&USE_INCLUDE))
			{
			auto t=deps.first->target();
			proc(t->nameGet(),rel);
			}
		if(rel==Dependency::Relation::INCLUDE_GENERATED && (use_flags&USE_INCLUDE_GENERATED))
			{
			auto name_full=dircat(target_dir,deps.first->target()->nameGet());
			proc(name_full.c_str(),rel);
			}
		if(rel==Dependency::Relation::GENERATED && (use_flags&USE_GENERATED))
			{
			auto name_full=dircat(target_dir,deps.first->target()->nameGet());
			proc(name_full.c_str(),rel);
			}
		if(rel==Dependency::Relation::FILE && (use_flags&USE_FILE))
			{
			auto t=deps.first->target();
			proc(t->nameGet(),rel);
			}
		if(rel==Dependency::Relation::INTERNAL && (use_flags&USE_INTERNAL))
			{
			auto t=deps.first->target();
			proc(t->nameGet(),rel);
			}	
		if(rel==Dependency::Relation::LEAF && (use_flags&USE_LEAF))
			{
			auto t=deps.first->target();
			proc(t->nameGet(),rel);
			}	

		++deps.first;
		}
	}



static std::vector<TargetCxxCompiler::FileInfo> depstringCreate(
	 std::vector<std::string>& strings_temp
	,const char* target_dir
	,Twins<const Dependency*> deps)
	{
	std::vector<TargetCxxCompiler::FileInfo> ret;
	while(deps.first!=deps.second)
		{
		switch(deps.first->relationGet())
			{
			case Dependency::Relation::EXTERNAL:
				{
				auto t=deps.first->target();
				ret.push_back({t->nameGet(),TargetCxxCompiler::FileUsage::LIB_EXTERNAL});
				}
				break;

			case Dependency::Relation::IMPLEMENTATION:
				{
				auto target_rel=dynamic_cast<const TargetCxx*>(deps.first->target());
				if(target_rel && target_rel->typeGet()!=TargetCxx::Type::INCLUDE)
					{
					auto name_full=dircat(target_dir,target_rel->nameGet());
					strings_temp.push_back(std::move(name_full));
					ret.push_back({strings_temp.back().c_str(),TargetCxxCompiler::FileUsage::NORMAL});
					}
				}
				break;

			case Dependency::Relation::INCLUDE_GENERATED:
				{
			//TODO: If target is application/dll, we should only add includes from main. See
			//applicationUpToDate
				auto name_full=dircat(target_dir,deps.first->target()->nameGet());
				strings_temp.push_back(std::move(name_full));
				ret.push_back({strings_temp.back().c_str(),TargetCxxCompiler::FileUsage::INCLUDE_EXTRA});
				}
				break;

			default:
				break;
			}
		++deps.first;
		}
	return std::move(ret);
	}


static void optionsCollect(Twins<const Dependency*> deps
	,TargetCxxOptions& options_out)
	{
	while(deps.first!=deps.second)
		{
		switch(deps.first->relationGet())
			{
			case Dependency::Relation::INCLUDE:
				{
				auto target_rel=dynamic_cast<const TargetCxx*>(deps.first->target());
				if(target_rel)
					{options_out.configAppend( target_rel->optionsExtraGet() );}
				}
				break;

			default:
				break;
			}
		++deps.first;
		}
	}

static std::vector<TargetCxxCompiler::FileInfo> depstringCreateAr(
	 std::vector<std::string>& strings_temp
	,const char* target_dir
	,Twins<const Dependency*> dependency_list_full)
	{
	std::vector<TargetCxxCompiler::FileInfo> ret;
	while(dependency_list_full.first!=dependency_list_full.second)
		{
		switch(dependency_list_full.first->relationGet())
			{
			case Dependency::Relation::IMPLEMENTATION:
				{
				auto target_rel=dynamic_cast<const TargetCxx*>(dependency_list_full.first->target());
				if(target_rel && target_rel->typeGet()!=TargetCxx::Type::INCLUDE)
					{
					auto name_full=dircat(target_dir,target_rel->nameGet());
					strings_temp.push_back(std::move(name_full));
					ret.push_back({strings_temp.back().c_str(),TargetCxxCompiler::FileUsage::NORMAL});
					}
				}
				break;

			default:
				break;
			}
		++dependency_list_full.first;
		}
	return std::move(ret);
	}

static void includeBuild(Twins<const Dependency*> dependency_list
	,const char* source_name,const char* name,const char* target_dir)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		auto t=dynamic_cast<const TargetCxx*>(dependency_list.first->target());
		if(t!=nullptr)
			{
			if(t->typeGet()==TargetCxx::Type::INCLUDE)
				{
				auto src=t->sourceNameGet();
				auto dest=dircat(target_dir,t->nameGet());
				if(FileUtils::newer(src,dest.c_str()))
					{FileUtils::copyFilter(src,dest.c_str(),"^[[:space:]]*//@");}
				}
			}
		++dependency_list.first;
		}
	FileUtils::copyFilter(source_name,name,"^[[:space:]]*//@");
	}

void TargetCxx::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	auto name_full=dircat(target_dir,nameGet());

	switch(m_type)
		{
		case Type::OBJECT:
			{
			auto options_extra=m_options_extra;
			optionsCollect(dependency_list_full,options_extra);
			std::vector<std::string> strings_temp(dependency_list.second - dependency_list.first);
			auto depfiles=depstringCreate(strings_temp,target_dir,dependency_list);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			r_compiler.compileObject(sourceNameGet(),inDirGet(),{deps_begin,deps_end},name_full.c_str(),options_extra);
			}
			break;
		case Type::INCLUDE_LIB:
			includeBuild(dependency_list,sourceNameGet(),name_full.c_str(),target_dir);
			break;
		case Type::APPLICATION:
			{
			std::vector<std::string> strings_temp(dependency_list_full.second - dependency_list_full.first);
			auto depfiles=depstringCreate(strings_temp,target_dir,dependency_list_full);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);

		//	-flto cannot be used with .incbin, and if some module is compiled with
		//	-flto, so must the application. Therfore do not do this for now
		//	auto options_extra=m_options_extra;
		//	optionsCollect(dependency_list_full,options_extra);
			r_compiler.compileApplication(sourceNameGet(),inDirGet(),{deps_begin,deps_end},name_full.c_str(),m_options_extra);
			}
			break;
		case Type::LIB_DYNAMIC:
			{
			std::vector<std::string> strings_temp;
			strings_temp.reserve(dependency_list_full.second - dependency_list_full.first);
			auto depfiles=depstringCreate(strings_temp,target_dir,dependency_list_full);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);
		//	-flto cannot be used with .incbin, and if some module is compiled with
		//	-flto, so must the application. Therfore do not do this for now
		//	auto options_extra=m_options_extra;
		//	optionsCollect(dependency_list_full,options_extra);
			r_compiler.compileDll(sourceNameGet(),inDirGet(),{deps_begin,deps_end},name_full.c_str(),m_options_extra);
			}
			break;
		case Type::LIB_STATIC:
			{
			std::vector<std::string> strings_temp;
			strings_temp.reserve(dependency_list_full.second - dependency_list_full.first);
			auto depfiles=depstringCreateAr(strings_temp,target_dir,dependency_list_full);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);
			r_compiler.compileLibrary(sourceNameGet(),inDirGet(),{deps_begin,deps_end},name_full.c_str(),m_options_extra);
			}
			break;

		case Type::INCLUDE:
			break;
		default:
			WriteBuffer(StdStream::error()).write(name_full.c_str())
				.write(": Target type ").write(static_cast<size_t>(m_type)).write(" not handled\n");
		}
	}


static bool objectUpToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_name_full
	,const char* target_dir)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		auto dep=dependency_list.first;
		switch(dep->relationGet())
			{
			case Dependency::Relation::INCLUDE_GENERATED:
			case Dependency::Relation::GENERATED:
				if( FileUtils::newer(dircat(target_dir,dep->nameGet()).c_str(),target_name_full))
					{return 0;}
				break;
			default:
				if(FileUtils::newer(dep->nameGet(),target_name_full))
					{return 0;}
			}
		++dependency_list.first;
		}
	return 1;
	}

static bool includeLibUpToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_name_full
	,const char* target_dir)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		auto t=dynamic_cast<const TargetCxx*>(dependency_list.first->target());
		if(t!=nullptr)
			{
			if(t->typeGet()==TargetCxx::Type::INCLUDE)
				{
				if(FileUtils::newer(t->sourceNameGet(),dircat(target_dir,t->nameGet()).c_str()))
					{return 0;}
				}
			}
		++dependency_list.first;
		}
	return 1;
	}

static bool applicationUpToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_name_full
	,const char* target_dir)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		auto dep=dependency_list.first;
		switch(dep->relationGet())
			{
			case Dependency::Relation::GENERATED:
				if( FileUtils::newer(dircat(target_dir,dep->nameGet()).c_str(),target_name_full ))
					{return 0;}
				break;
			case Dependency::Relation::EXTERNAL:
				break;

			default:
				if(FileUtils::newer(dep->nameGet(),target_name_full))
					{return 0;}
			}
		++dependency_list.first;
		}

	while(dependency_list_full.first!=dependency_list_full.second)
		{
		auto target_rel=dynamic_cast<const TargetCxx*>(dependency_list_full.first->target());
		if(target_rel!=nullptr)
			{
			if(target_rel->typeGet()!=TargetCxx::Type::INCLUDE)
				{
				auto target_rel_name_full=dircat(target_dir,target_rel->nameGet());
				if(FileUtils::newer(target_rel_name_full.c_str(),target_name_full))
					{return 0;}
				}
			}
		++dependency_list_full.first;
		}
	return 1;
	}

bool TargetCxx::upToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir) const
	{
	auto name_full=dircat( target_dir,nameGet() );

	if(FileUtils::newer(sourceNameGet(),name_full.c_str()))
		{return 0;}

	switch(m_type)
		{
		case Type::INCLUDE_LIB:
			return includeLibUpToDate(dependency_list,dependency_list_full
				,name_full.c_str(),target_dir);
		case Type::OBJECT:
			return objectUpToDate(dependency_list,dependency_list_full
				,name_full.c_str(),target_dir);
		case Type::INCLUDE:
			return 1;

		case Type::LIB_STATIC:
		case Type::LIB_DYNAMIC:
		case Type::APPLICATION:
			return applicationUpToDate(dependency_list,dependency_list_full
				,name_full.c_str()
				,target_dir);
		default:
			break;
		}

	return 1;
	}

TargetCxx* TargetCxx::create(const ResourceObject& obj
	,const TargetCxxCompiler& compiler,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{
	return new TargetCxx(obj,compiler,name_src,in_dir,root,id,line_count);
	}

void TargetCxx::destroy() noexcept
	{
	delete this;
	}

