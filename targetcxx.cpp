//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "targetcxxcompiler.hpp"
#include "fileutils.hpp"
#include "pathutils.hpp"
#include "stdstream.hpp"
#include "writebuffer.hpp"
#include "parametersetmapfixed.hpp"
#include "readbuffer.hpp"
#include "thread.hpp"
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

	exceptionRaise(ErrorMessage("#0;: Unknown target type #1;."	,{name_src,type}));
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
		}
	return nullptr;
	}




typedef ParameterSetMapFixed<
	 Stringkey("action")
	,Stringkey("libname")> PkgConfigParams;

namespace
	{
	class ReadCallback
		{
		public:
			explicit ReadCallback(DataSource* src):r_src(src)
				{}

			void operator()()
				{
				WriteBuffer wb(StdStream::error());
				try
					{
					ReadBuffer rb(*r_src);
					while(!rb.eof())
						{
						wb.write(rb.byteRead());
						}
					}
				catch(const ErrorMessage& message)
					{wb.write("Error: ").write(message.messageGet()).write("\n");}
				}

		private:
			DataSource* r_src;
		};
	}

static void pkgconfigIncludesRead(Pipe& cflags_only_i,TargetCxxOptions& options_out)
	{
	auto standard_error=cflags_only_i.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_output=cflags_only_i.stdoutCapture();
	ReadBuffer rb(*standard_output.get());
	WriteBuffer wb(StdStream::output());
	enum class State:unsigned int{NORMAL,I,IDIR,REM1,REM2};
	auto state=State::NORMAL;
	std::string reminder;
	std::string dir;
	while(!rb.eof())
		{
		auto ch_in=rb.byteRead();
		switch(state)
			{
			case State::NORMAL:
				{
				switch(ch_in)
					{
					case '-':
						state=State::I;
						break;
					default:
						break;
					}
				}
				break;
			case State::I:
				{
				switch(ch_in)
					{
					case 'I':
						state=State::IDIR;
						break;
					default:
						exceptionRaise(ErrorMessage("Unexpected data in `pkg-config` output",{}));
					}
				}
				break;
			case State::IDIR:
				{
				switch(ch_in)
					{
					case '\n':
					case ' ':
						state=State::REM1;
						reminder+=ch_in;
						break;
					default:
						dir+=ch_in;
					}
				}
				break;
			case State::REM1:
				switch(ch_in)
					{
					case '-':
						reminder+=ch_in;
						state=State::REM2;
						break;
					default:
						dir+=reminder;
						reminder.clear();
						state=State::IDIR;
						break;
					}
				break;
			case State::REM2:
				switch(ch_in)
					{
					case 'I':
						reminder.clear();
						state=State::IDIR;
						options_out.includedirNoscanAppend(dir.c_str());
						dir.clear();
						break;
					default:
						dir+=reminder;
						reminder.clear();
						state=State::IDIR;
						break;
					}
				break;
			}
		}
	options_out.includedirNoscanAppend(dir.c_str());
	}

static void pkgconfigCflagsRead(Pipe& cflags,TargetCxxOptions& options_out)
	{
	auto standard_error=cflags.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_output=cflags.stdoutCapture();
	ReadBuffer rb(*standard_output.get());
	WriteBuffer wb(StdStream::output());
	while(!rb.eof())
		{
		wb.write(rb.byteRead());
		}
	}

static void pkgconfig(const Command& cmd,const char* libname,TargetCxx& target,TargetCxxOptions& options_out)
	{
	PkgConfigParams params;
	const ParameterSet* paramsets[]={&params};
	params.get<Stringkey("libname")>().push_back(std::string(libname));

	params.get<Stringkey("action")>().push_back(std::string("--cflags-only-I"));
	auto pkgconfig=cmd.execute(Pipe::REDIRECT_STDERR|Pipe::REDIRECT_STDOUT,{paramsets,paramsets + 1});
	pkgconfigIncludesRead(pkgconfig,options_out);
	auto status=pkgconfig.exitStatusGet();
	if(status!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to find information about "
			"the library #1;",{target.sourceNameGet(),libname}));
		}

	params.get<Stringkey("action")>()[0]=std::string("--cflags-only-other");
	pkgconfig=cmd.execute(Pipe::REDIRECT_STDERR|Pipe::REDIRECT_STDOUT,{paramsets,paramsets + 1});
	pkgconfigCflagsRead(pkgconfig,options_out);
	status=pkgconfig.exitStatusGet();
	if(status!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to find information about "
			"the library #1;",{target.sourceNameGet(),libname}));
		}
	}

void TargetCxx::pkgconfig(const ResourceObject& pkgconfig_libs)
	{
	auto cmd=(r_compiler.optionsGet()|m_options_extra).pkgconfigGet();
	auto N=pkgconfig_libs.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		::pkgconfig(cmd,static_cast<const char*>(pkgconfig_libs.objectGet(k)),*this,m_options_extra);
		}
	}




TargetCxx::TargetCxx(const ResourceObject& obj,const TargetCxxCompiler& compiler
	,const char* name_src,const char* in_dir,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,id,line_count)
	,r_compiler(compiler)
	{
	m_type=type(name_src,static_cast<const char*>( obj.objectGet("type") ));

	if(obj.objectExists("cxxoptions"))
		{
		m_options_extra=TargetCxxOptions(obj.objectGet("cxxoptions"));
		}
	if(obj.objectExists("pkgconfig_libs"))
		{
		pkgconfig(obj.objectGet("pkgconfig_libs"));
		}
	}

void TargetCxx::dumpDetails(ResourceObject& target) const
	{
	target.objectSet("type",ResourceObject(type(typeGet())));

	ResourceObject cxxoptions(ResourceObject::Type::OBJECT);
	m_options_extra.configDump(cxxoptions);
	target.objectSet("cxxoptions",std::move(cxxoptions));
	}



static std::vector<TargetCxxCompiler::FileInfo> depstringCreate(
	 std::vector<std::string>& strings_temp
	,const char* target_dir
	,Twins<const Dependency*> dependency_list_full)
	{
	std::vector<TargetCxxCompiler::FileInfo> ret;
	while(dependency_list_full.first!=dependency_list_full.second)
		{
		switch(dependency_list_full.first->relationGet())
			{
			case Dependency::Relation::EXTERNAL:
				{
				auto t=dependency_list_full.first->target();
				ret.push_back({t->nameGet(),TargetCxxCompiler::FileUsage::LIB_EXTERNAL});
				}
				break;

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

void TargetCxx::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	auto name_full=dircat(target_dir,nameGet());

	switch(m_type)
		{
		case Type::OBJECT:
			r_compiler.compileObject(sourceNameGet(),name_full.c_str(),m_options_extra);
			break;
		case Type::APPLICATION:
			{
			std::vector<std::string> strings_temp;
			auto depfiles=depstringCreate(strings_temp,target_dir,dependency_list_full);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);
			r_compiler.compileApplication(sourceNameGet(),{deps_begin,deps_end},name_full.c_str(),m_options_extra);
			}
			break;
		case Type::LIB_DYNAMIC:
			{
			std::vector<std::string> strings_temp;
			auto depfiles=depstringCreate(strings_temp,target_dir,dependency_list_full);
			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);
			r_compiler.compileDll(sourceNameGet(),{deps_begin,deps_end},name_full.c_str(),m_options_extra);
			}
			break;
		case Type::INCLUDE:
			break;
		default:
			WriteBuffer(StdStream::error()).write(name_full.c_str())
				.write(": Target not handled\n");
		}
	}


static bool objectUpToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_name_full)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		if(FileUtils::newer(dependency_list.first->nameGet()
			,target_name_full))
			{return 0;}
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
		if(dependency_list.first->relationGet()!=Dependency::Relation::EXTERNAL)
			{
			if(FileUtils::newer(dependency_list.first->nameGet()
				,target_name_full))
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
		case Type::OBJECT:
			return objectUpToDate(dependency_list,dependency_list_full
				,name_full.c_str());
		case Type::INCLUDE:
			return 1;

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
	,const char* in_dir,size_t id,size_t line_count)
	{
	return new TargetCxx(obj,compiler,name_src,in_dir,id,line_count);
	}

void TargetCxx::destroy() noexcept
	{
	delete this;
	}

