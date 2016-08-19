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

static unsigned long long int cxxversionDefaultGet(Pipe& versionget)
	{
	auto standard_error=versionget.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});

	auto standard_output=versionget.stdoutCapture();
	TargetCxxPPTokenizer cxxtok(*standard_output.get());
	TargetCxxPPTokenizer::Token tok;
	enum class Mode:unsigned int{NORMAL,DEFINE,DONE};
	auto state=Mode::NORMAL;
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
				if(tok.value=="__cplusplus")
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
	,Twins<const ParameterSet* const*> paramset)
	{
	auto versionget=versionquery.execute(Pipe::REDIRECT_STDOUT|Pipe::REDIRECT_STDERR
		,paramset);

	auto ret=cxxversionDefaultGet(versionget);
	auto status=versionget.exitStatusGet();
	if(status!=0)
		{
		exceptionRaise(ErrorMessage("It was not possible to determine the "
			"default C++ version. The compiler returned status code #0;",{status}));
		}
	return ret;
	}

TargetCxxCompiler::TargetCxxCompiler(const TargetCxxOptions& options
	,const ParameterSet& sysvars):r_options(options),m_cxxversion_default(0)
	{
	r_paramset.push_back(&sysvars);
	}

typedef ParameterSetMapFixed<
	 Stringkey("cxxversion")
	,Stringkey("dependencies")
	,Stringkey("includedir")
	,Stringkey("libdir")
	,Stringkey("target")
	,Stringkey("source")
	,Stringkey("cflags_extra")> CompilerParameters;

static const char* cxxNameGet(unsigned long long int cxxversion)
	{
	if(cxxversion < 201103L)
		{return "c++03";}
	if(cxxversion < 201402L)
		{return "c++11";}
	return "c++14";
	}

static std::string
cxxVersionString(const char* stdprefix,unsigned long long int cxxversion)
	{
	std::string ret(stdprefix);
	ret+=cxxNameGet(cxxversion);
	return ret;
	}

unsigned long long int TargetCxxCompiler::cxxversionDefaultGet() const
	{
	if(m_cxxversion_default==0)
		{
		m_cxxversion_default=::cxxversionDefaultGet(r_options.versionqueryGet()
			,{r_paramset.data(), r_paramset.data() + r_paramset.size()});
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
		}
	return std::string(fileinfo.filename);
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

void TargetCxxCompiler::execute(const Command& cmd,const char* source
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	CompilerParameters cxxparams;
	cxxparams.get<Stringkey("target")>().push_back(dest);
	cxxparams.get<Stringkey("source")>().push_back(source);

	auto options_result=r_options|options_extra;
	auto cxxversion_min=options_result.cxxversionMinGet();
	auto cxxversion_max=options_result.cxxversionMaxGet();
	if(cxxversion_min > cxxversionDefaultGet())
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_min));
		}
	if(cxxversion_max < cxxversionDefaultGet())
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_max));
		}


	argvBuild(cxxparams.get<Stringkey("includedir")>()
		,options_result.includedirGet(),options_result.includedirFormatGet());
	argvBuild(cxxparams.get<Stringkey("includedir")>()
		,options_result.includedirNoscanGet(),options_result.includedirFormatGet());
	argvBuild(cxxparams.get<Stringkey("cflags_extra")>()
		,options_result.cflagsExtraGet(),options_result.cflagsFormatGet());

	if(dependencies.first!=dependencies.second)
		{
		auto& deps=cxxparams.get<Stringkey("dependencies")>();
		do
			{
			deps.push_back(makeDepitemString(*dependencies.first,options_result));
			++dependencies.first;
			}
		while(dependencies.first!=dependencies.second);
		argvBuild(cxxparams.get<Stringkey("libdir")>()
			,options_result.libdirGet(),options_result.libdirFormatGet());
		}

	const ParameterSet* paramset[]={&cxxparams};
	auto compiler=cmd.execute(Pipe::REDIRECT_STDERR,{paramset, paramset + 1 });
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

void TargetCxxCompiler::compileObject(const char* source,const char* dest
	,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.objcompileGet(),source,{nullptr,nullptr},dest,options_extra);
	}

void TargetCxxCompiler::compileApplication(const char* source
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.appcompileGet(),source,dependencies,dest,options_extra);
	}

void TargetCxxCompiler::compileDll(const char* source
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.dllcompileGet(),source,dependencies,dest,options_extra);
	}

void TargetCxxCompiler::compileLibrary(const char* source
	,Twins<const FileInfo*> dependencies
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	execute(r_options.libcompileGet(),source,dependencies,dest,options_extra);
	}

