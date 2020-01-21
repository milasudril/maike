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
#include "../readbuffer.hpp"
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
	dependencyAdd(Dependency(cmd.nameGet(),Dependency::Relation::TOOL));
	auto N=pkgconfig_libs.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		r_compiler.pkgconfigAsk(static_cast<const char*>(pkgconfig_libs.objectGet(k)),sourceNameGet())
			.optionsPush(m_options_extra,m_options_extra_local).dependenciesPush(*this);
		}
	}


static const char* chooseFirstNonEmpty(const char* a)
	{return a;}

template<class ... Args>
static const char* chooseFirstNonEmpty(const char* a,Args... vals)
	{
	if(*a=='\0')
		{return chooseFirstNonEmpty(vals...);}
	return a;
	}

static Dependency makeDependencyOnCompiler(const TargetCxxOptions& options_extra_local
	,const TargetCxxOptions& options_extra
	,const TargetCxxOptions& optionsCompiler
	,TargetCxx::Type targetType
	,const char* root)
	{
	const char* depname="";
	switch(targetType)
		{
	//	Assume that the complete toolchain is installed if the compiler is
	//	installed. This means that we do not need to check for the preprocessor
	//	or the assembler.
		case TargetCxx::Type::OBJECT:
			depname=chooseFirstNonEmpty(options_extra_local.objcompileGet().nameGet()
				,options_extra.objcompileGet().nameGet()
				,optionsCompiler.objcompileGet().nameGet());
			break;
		case TargetCxx::Type::APPLICATION:
			depname=chooseFirstNonEmpty(options_extra_local.appcompileGet().nameGet()
				,options_extra.appcompileGet().nameGet()
				,optionsCompiler.appcompileGet().nameGet());
			break;
		case TargetCxx::Type::LIB_DYNAMIC:
			depname=chooseFirstNonEmpty(options_extra_local.dllcompileGet().nameGet()
				,options_extra.dllcompileGet().nameGet()
				,optionsCompiler.dllcompileGet().nameGet());
			break;
		case TargetCxx::Type::LIB_STATIC:
			depname=chooseFirstNonEmpty(options_extra_local.libcompileGet().nameGet()
				,options_extra.libcompileGet().nameGet()
				,optionsCompiler.libcompileGet().nameGet());
			break;
		case TargetCxx::Type::INCLUDE_LIB:
			break;
		case TargetCxx::Type::INCLUDE:
			break;
		}
	return Dependency(depname,root,Dependency::Relation::TOOL);
	}

TargetCxx::TargetCxx(const ResourceObject& obj,const TargetCxxCompiler& compiler
	,const char* name_src,const char* in_dir,const char* root,size_t id
	,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,m_autorun(0),r_compiler(compiler)
	{
	m_type=type(name_src,static_cast<const char*>( obj.objectGet("type") ));

	if(obj.objectExists("cxxoptions"))
		{m_options_extra=TargetCxxOptions(obj.objectGet("cxxoptions"));}
	if(obj.objectExists("cxxoptions_local"))
		{m_options_extra_local=TargetCxxOptions(obj.objectGet("cxxoptions_local"));}
	if(obj.objectExists("pkgconfig_libs"))
		{pkgconfig(obj.objectGet("pkgconfig_libs"));}
	if(obj.objectExists("include_targets"))
		{
		auto itargets=obj.objectGet("include_targets");
		auto N=itargets.objectCountGet();
		for(decltype(N) k=0;k<N;++k)
			{
			auto dep_name=static_cast<const char*>(itargets.objectGet(k));
			Dependency dep( dircat(in_dir,dep_name).c_str(),root,Dependency::Relation::INCLUDE_EXTRA );
			dependencyAdd( std::move(dep) );
			}
		}

	auto& optionsCompiler=r_compiler.optionsGet();
	if(obj.objectExists("autorun"))
		{
		if(!(m_type==Type::APPLICATION || m_type==Type::LIB_DYNAMIC))
			{exceptionRaise(ErrorMessage{"#0;: Only applications and dynamic libraries can have the autorun flag",{name_src}});}
		m_autorun=static_cast<long long int>(obj.objectGet("autorun"));

		if(m_autorun)
			{
			auto depname=chooseFirstNonEmpty(m_options_extra_local.autorunLauncherGet().nameGet()
				,optionsCompiler.autorunLauncherGet().nameGet());
			if(*depname!='\0')
				{dependencyAdd(Dependency(depname,root,Dependency::Relation::TOOL));}
			}
		}

	if(*m_options_extra_local.modeGet()=='\0')
		{m_options_extra_local.modeSet(optionsCompiler.modeGet());}

	if(m_type!=Type::INCLUDE && m_type!=Type::INCLUDE_LIB)
		{dependencyAdd(makeDependencyOnCompiler(m_options_extra_local,m_options_extra,optionsCompiler,m_type,root));}
	}

bool TargetCxx::generated() const noexcept
	{
	switch(m_type)
		{
		case Type::OBJECT:
		case Type::APPLICATION:
		case Type::LIB_DYNAMIC:
		case Type::LIB_STATIC:
		case Type::INCLUDE_LIB:
			return 1;
		case Type::INCLUDE:
			return 0;
		}
	return 0;
	}

void TargetCxx::dumpDetails(ResourceObject& target) const
	{
	target.objectSet("type",target.create(type(typeGet())));

		{
		auto cxxoptions=target.createObject();
		m_options_extra.configDump(cxxoptions);
		target.objectSet("cxxoptions",std::move(cxxoptions));
		}

		{
		auto cxxoptions=target.createObject();
		m_options_extra_local.configDump(cxxoptions);
		target.objectSet("cxxoptions_local",std::move(cxxoptions));
		}

	if(m_type==Type::APPLICATION || m_type==Type::LIB_DYNAMIC)
		{
		target.objectSet("autorun",target.create(static_cast<long long int>(m_autorun)));
		}

	}

static void optionsCollect(Twins<const Dependency*> deps
	,TargetCxxOptions& options_out)
	{
	while(deps.first!=deps.second)
		{
		switch(deps.first->relationGet())
			{
			case Dependency::Relation::INCLUDE:
			case Dependency::Relation::INCLUDE_EXTRA:
			case Dependency::Relation::IMPLEMENTATION:
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
					{
				//TODO: We need to add #include for generated include files
					FileUtils::copyFilter(src,dest.c_str(),"^[[:space:]]*//@");
					}
				}
			}
		++dependency_list.first;
		}
//TODO: We need to add #include for generated include files
	FileUtils::copyFilter(source_name,name,"^[[:space:]]*//@");
	}

template<class T,class Container>
static T& push_back(T&& obj,Container& c)
	{
#ifndef NDEBUG
	auto data_old=c.data();
#endif
	c.push_back(std::move(obj));
	assert(c.data()==data_old);
	return c.back();
	}

template<class T,class U>
T map(U);

template<>
TargetCxxCompiler::FileUsage map<TargetCxxCompiler::FileUsage,Dependency::Relation>(Dependency::Relation rel)
	{
	switch(rel)
		{
//NORMAL,LIB_INTERNAL,LIB_EXTERNAL,INCLUDE_EXTRA
		case Dependency::Relation::IMPLEMENTATION:
			return TargetCxxCompiler::FileUsage::NORMAL;
		case Dependency::Relation::EXTERNAL:
			return TargetCxxCompiler::FileUsage::LIB_EXTERNAL;
		case Dependency::Relation::INCLUDE_EXTRA:
			return TargetCxxCompiler::FileUsage::INCLUDE_EXTRA;
		default:
			assert(!("Intenal error"));
		}
	}

void TargetCxx::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	auto name_full=dircat(target_dir,nameGet());

	std::vector<std::string> strings_temp;
	std::vector<TargetCxxCompiler::FileInfo> depfiles;

	auto depstring_create=[&strings_temp,&depfiles](const char* depname,Dependency::Relation rel)
		{
		depfiles.push_back(
			{
			 push_back(std::string(depname),strings_temp).c_str()
			,map<TargetCxxCompiler::FileUsage>(rel)
			});
		return 1;
		};

	switch(m_type)
		{
		case Type::OBJECT:
			{
			auto options_extra=m_options_extra;
			optionsCollect(dependency_list_full,options_extra);
			options_extra.configAppend(m_options_extra_local);
			strings_temp.reserve(dependency_list.second - dependency_list.first);
			dependenciesProcess(target_dir,dependency_list,USE_INCLUDE_EXTRA,depstring_create);
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
			auto N=(dependency_list_full.second - dependency_list_full.first);
			N+=(dependency_list.second - dependency_list.first);
			strings_temp.reserve(N);
			dependenciesProcess(target_dir,dependency_list,USE_INCLUDE_EXTRA,depstring_create);
			dependenciesProcess(target_dir,dependency_list_full,USE_EXTERNAL|USE_IMPLEMENTATION,depstring_create);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);
			auto options_extra=m_options_extra;
			optionsCollect(dependency_list_full,options_extra);
			options_extra.configAppend(m_options_extra_local);
			r_compiler.compileApplication(sourceNameGet(),inDirGet(),{deps_begin,deps_end},name_full.c_str(),options_extra);
			if(m_autorun)
				{r_compiler.runTargetApp(sourceNameGet(),inDirGet(), name_full.c_str(), m_options_extra_local);}
			}
			break;
		case Type::LIB_DYNAMIC:
			{
			auto N=(dependency_list_full.second - dependency_list_full.first);
			N+=(dependency_list.second - dependency_list.first);
			strings_temp.reserve(N);
			dependenciesProcess(target_dir,dependency_list,USE_INCLUDE_EXTRA,depstring_create);
			dependenciesProcess(target_dir,dependency_list_full,USE_EXTERNAL|USE_IMPLEMENTATION,depstring_create);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);

			auto options_extra=m_options_extra;
			optionsCollect(dependency_list_full,options_extra);
			options_extra.configAppend(m_options_extra_local);

			r_compiler.compileDll(sourceNameGet(),inDirGet(),{deps_begin,deps_end},name_full.c_str(),options_extra);
			if(m_autorun)
				{r_compiler.runTargetDll(sourceNameGet(),inDirGet(),name_full.c_str(), m_options_extra_local);}
			}
			break;
		case Type::LIB_STATIC:
			{
			strings_temp.reserve(dependency_list_full.second - dependency_list_full.first);
			dependenciesProcess(target_dir,dependency_list_full,USE_IMPLEMENTATION,depstring_create);
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

bool TargetCxx::upToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir) const
	{
	auto name_full=dircat( target_dir,nameGet() );

	if(FileUtils::newer(sourceNameGet(),name_full.c_str()))
		{return 0;}

	auto up_to_date=[&name_full](const char* name,Dependency::Relation)
		{return !FileUtils::newer(name,name_full.c_str());};

	switch(m_type)
		{
		case Type::OBJECT:
			return dependenciesProcess(target_dir,dependency_list
				,USE_INCLUDE_EXTRA|USE_INCLUDE|USE_MISC|USE_TOOL
				,up_to_date);

		case Type::INCLUDE_LIB:
			return dependenciesProcess(target_dir,dependency_list
				,USE_INCLUDE|USE_INCLUDE_EXTRA,up_to_date);

		case Type::INCLUDE:
			return 1;

		case Type::LIB_STATIC:
		case Type::LIB_DYNAMIC:
		case Type::APPLICATION:
			return dependenciesProcess(target_dir,dependency_list
				,USE_INCLUDE_EXTRA|USE_INCLUDE|USE_MISC|USE_TOOL,up_to_date)
				&& dependenciesProcess(target_dir,dependency_list_full,USE_IMPLEMENTATION,up_to_date);

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

