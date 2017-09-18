//@	{
//@	 "targets":[{"name":"targetcxxcompiler.o","type":"object"}]
//@	}

#include "targetcxxcompiler.hpp"
#include "targetcxxoptions.hpp"
#include "targetcxxpptokenizer.hpp"
#include "../dependency.hpp"
#include "../fileutils.hpp"
#include "../readbuffer.hpp"
#include "../thread.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../exceptionhandler.hpp"
#include "../parametersetmapfixed.hpp"
#include "../stdstream.hpp"
#include "../writebuffer.hpp"
#include <cstring>

using namespace Maike;

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
					{wb.write("Error: ").write(message.messageGet());}
				}

		private:
			DataSource* r_src;
		};
	}
	
static const char* macroGet(const char* mode)
	{
	if(strcmp(mode,"c++")==0 || strcmp(mode,"C++")==0)
		{return "__cplusplus";}
	else
	if(strcmp(mode,"C")==0 || strcmp(mode,"v")==0)
		{return "__STDC_VERSION__";}
	exceptionRaise(ErrorMessage("The current language (#0;) is not supported.",{mode}));
	}

static unsigned long long int cxxversionDefaultGet(Pipe& versionget,const char* mode)
	{
	auto standard_error=versionget.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});

	auto standard_output=versionget.stdoutCapture();
	TargetCxxPPTokenizer cxxtok(*standard_output.get());
	TargetCxxPPTokenizer::Token tok;
	enum class Mode:unsigned int{NORMAL,DEFINE,DONE};
	auto state=Mode::NORMAL;
	auto look_for=macroGet(mode);
	while(cxxtok.read(tok))
		{
		switch(state)
			{
			case Mode::NORMAL:
				switch(tok.type)
					{
					case TargetCxxPPTokenizer::Token::Type::DIRECTIVE:
						if(tok.value=="define")
							{state=Mode::DEFINE;}
						break;
					default:
						break;
					}
				break;
			case Mode::DEFINE:
				if(tok.value==look_for)
					{state=Mode::DONE;}
				else
					{state=Mode::NORMAL;}
				break;

			case Mode::DONE:
				return atoll(tok.value.c_str());
			}
		}
	return 0;
	}


static unsigned long long int cxxversionDefaultGet(const Command& versionquery
	,Twins<const ParameterSet* const*> paramset,const char* mode)
	{
	auto versionget=versionquery.execute(Pipe::REDIRECT_STDOUT|Pipe::REDIRECT_STDERR
		,paramset);

	auto ret=cxxversionDefaultGet(versionget,mode);
	auto status=versionget.exitStatusGet();
	if(status!=0)
		{
		exceptionRaise(ErrorMessage("It was not possible to determine the "
			"default C++ version. The compiler returned status code #0;",{status}));
		}
	
	if(ret==0)
		{
		exceptionRaise(ErrorMessage("The current language (#0;) appears to be "
			"unsupported by the chosen compiler.",{mode}));
		}
	
	return ret;
	}

TargetCxxCompiler::TargetCxxCompiler(const TargetCxxOptions& options
	,const ParameterSet& sysvars):r_options(options),m_cxxversion_default(0)
	{
	r_paramset=&sysvars;
	}

typedef ParameterSetMapFixed<
	 Stringkey("cxxversion")
	,Stringkey("dependencies")
	,Stringkey("includedir")
	,Stringkey("libdir")
	,Stringkey("target")
	,Stringkey("source")
	,Stringkey("cflags_extra")
	,Stringkey("iquote")
	,Stringkey("current_directory")
	,Stringkey("target_directory")
	,Stringkey("includes_extra")> CompilerParameters;

static const char* cxxNameGet(unsigned long long int cxxversion,const char* mode)
	{
	if(strcmp(mode,"c++")==0 || strcmp(mode,"C++")==0)
		{
		if(cxxversion < 201103L)
			{return "c++03";}
		if(cxxversion < 201402L)
			{return "c++11";}
		return "c++14";
		}
	else
	if(strcmp(mode,"C")==0 || strcmp(mode,"v")==0)
		{
		if(cxxversion < 199901)
			{return "c89";}
		if(cxxversion < 201102)
			{return "c99";}
		return "c11";
		}
	exceptionRaise(ErrorMessage("The current language (#0;) is not supported.",{mode}));
		
	}

static std::string
cxxVersionString(const char* stdprefix,unsigned long long int cxxversion,const char* mode)
	{
	std::string ret(stdprefix);
	ret+=cxxNameGet(cxxversion,mode);
	return ret;
	}

unsigned long long int TargetCxxCompiler::cxxversionDefaultGet(const char* mode) const
	{
	if(m_cxxversion_default==0)
		{
		auto p=&r_paramset;
		m_cxxversion_default=::cxxversionDefaultGet(r_options.versionqueryGet()
			,{p, p + 1},mode);
		}
	return m_cxxversion_default;
	}

static std::string placeholderSubstitute(const char* string_template
	,const char* substitute)
	{
	std::string ret;
	while(true)
		{
		auto ch_in=*string_template;
		switch(ch_in)
			{
			case '^':
				ret+=substitute;
				break;
			case '\0':
				return std::move(ret);
			default:
				ret+=ch_in;
			}
		++string_template;
		}
	}

static std::string makeDepitemString(const TargetCxxCompiler::FileInfo& fileinfo
	,const TargetCxxOptions& options)
	{
	switch(fileinfo.usage)
		{
		case TargetCxxCompiler::FileUsage::NORMAL:
			return std::string(fileinfo.filename);
		case TargetCxxCompiler::FileUsage::LIB_INTERNAL:
			return placeholderSubstitute(options.libintFormatGet(),fileinfo.filename);
		case TargetCxxCompiler::FileUsage::LIB_EXTERNAL:
			return placeholderSubstitute(options.libextFormatGet(),fileinfo.filename);
		default:
			return std::string("");
		}
	return std::string("");
	}

static std::string makeIncludeString(const TargetCxxCompiler::FileInfo& fileinfo
	,const TargetCxxOptions& options)
	{
	switch(fileinfo.usage)
		{
		case TargetCxxCompiler::FileUsage::INCLUDE_EXTRA:
			return placeholderSubstitute(options.includeFormatGet(),fileinfo.filename);
		default:
			return std::string("");
		}
	return std::string("");
	}

static void argvBuild(std::vector<std::string>& argv
	,Twins<const std::string*> strings_raw,const char* string_template)
	{
	while(strings_raw.first!=strings_raw.second)
		{
		argv.push_back(placeholderSubstitute(string_template,strings_raw.first->c_str()));
		++strings_raw.first;
		}
	}

namespace
	{
	class StringGet:public ParameterSet::ParameterProcessor
		{
		public:
			StringGet(std::string& ret):r_ret(ret)
				{}

		virtual void operator()(const char* element)
			{r_ret=std::string(element);}

		private:
			std::string& r_ret;
		};
	}

void TargetCxxCompiler::execute(const Command& cmd
	,const char* source
	,const char* in_dir
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	CompilerParameters cxxparams;

	std::string target_dir;
	r_paramset->parameterGet(Stringkey("target_directory"),StringGet(target_dir));

	std::string root;
	r_paramset->parameterGet(Stringkey("project_root"),StringGet(root));
	
	cxxparams.get<Stringkey("target")>().push_back(dest);
	cxxparams.get<Stringkey("source")>().push_back(source);
	cxxparams.get<Stringkey("current_directory")>().push_back(dircat(root,in_dir));
	cxxparams.get<Stringkey("target_directory")>().push_back(dircat(root,target_dir));

	auto options_result=r_options|options_extra;
	auto cxxversion_min=options_result.cxxversionMinGet();
	auto cxxversion_max=options_result.cxxversionMaxGet();

	if(cxxversion_min > cxxversionDefaultGet(options_result.modeGet()))
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_min
				,options_result.modeGet()));
		}
	if(cxxversion_max < cxxversionDefaultGet(options_result.modeGet()))
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_max
				,options_result.modeGet()));
		}


	argvBuild(cxxparams.get<Stringkey("includedir")>()
		,options_result.includedirGet(),options_result.includedirFormatGet());
	argvBuild(cxxparams.get<Stringkey("includedir")>()
		,options_result.includedirNoscanGet(),options_result.includedirFormatGet());
	argvBuild(cxxparams.get<Stringkey("cflags_extra")>()
		,options_result.cflagsExtraGet(),options_result.cflagsFormatGet());
	argvBuild(cxxparams.get<Stringkey("iquote")>()
		,options_result.iquoteGet(),options_result.iquoteFormatGet());

	if(dependencies.first!=dependencies.second)
		{
		auto& deps=cxxparams.get<Stringkey("dependencies")>();
		auto ptr=dependencies.first;
		do
			{
			auto str=makeDepitemString(*ptr,options_result);
			if(str.size()!=0)
				{deps.push_back(std::move(str));}
			++ptr;
			}
		while(ptr!=dependencies.second);
		if(deps.size()!=0)
			{
			argvBuild(cxxparams.get<Stringkey("libdir")>()
				,options_result.libdirGet(),options_result.libdirFormatGet());
			}
		
		auto& includes=cxxparams.get<Stringkey("includes_extra")>();
		ptr=dependencies.first;
		do
			{
			auto str=makeIncludeString(*ptr,options_result);
			if(str.size()!=0)
				{includes.push_back(std::move(str));}
			++ptr;
			}
		while(ptr!=dependencies.second);
		}

	const ParameterSet* paramset[]={r_paramset,&cxxparams};
	auto compiler=cmd.execute(Pipe::REDIRECT_STDERR
		,{paramset,paramset + 2 });
	auto stream=compiler.stderrCapture();
	ReadBuffer rb(*stream.get());
	WriteBuffer wb(StdStream::error());
	while(!rb.eof())
		{wb.write(rb.byteRead());}
	auto res=compiler.exitStatusGet();
	if(res!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
			"The compiler returned status code #2;",{source,dest,res}));
		}
	}

void TargetCxxCompiler::compileObject(const char* source,const char* in_dir
	,Twins<const FileInfo*> dependencies
	,const char* dest
	,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.objcompileGet(),source,in_dir,dependencies,dest,options_extra);
	}

void TargetCxxCompiler::compileApplication(const char* source,const char* in_dir
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.appcompileGet(),source,in_dir,dependencies,dest,options_extra);
	}

void TargetCxxCompiler::compileDll(const char* source,const char* in_dir
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.dllcompileGet(),source,in_dir,dependencies,dest,options_extra);
	}

void TargetCxxCompiler::compileLibrary(const char* source,const char* in_dir
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.libcompileGet(),source,in_dir,dependencies,dest,options_extra);
	}

const PkgConfigRequest& TargetCxxCompiler::pkgconfigAsk(const char* library,const char* context) const
	{
	auto key=Stringkey(library);
	auto i=m_pkgconfig_cache.find(key);
	if(i!=m_pkgconfig_cache.end())
		{return i->second;}

	auto ip=m_pkgconfig_cache.insert({key,PkgConfigRequest(r_options.pkgconfigGet(),library,context)});
	return ip.first->second;
	}

