//@	{
//@	 "targets":[{"name":"targetcxxcompiler.o","type":"object"}]
//@	}

#include "targetcxxcompiler.hpp"
#include "targetcxxoptions.hpp"
#include "dependency.hpp"
#include "fileutils.hpp"
#include "readbuffer.hpp"
#include "thread.hpp"
#include "targetcxxpptokenizer.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "parametersetmapfixed.hpp"
#include <cstdio>

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
				try
					{
					ReadBuffer rb(*r_src);
					while(!rb.eof())
						{
						auto ch_in=rb.byteRead();
						putchar(ch_in);
						}
					}
				catch(const ErrorMessage& message)
					{fprintf(stderr,"Error: %s\n",message.messageGet());}
				}

		private:
			DataSource* r_src;
		};
	}

static long int cxxversionDefaultGet(Pipe& versionget)
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


static long int cxxversionDefaultGet(const Command& versionquery
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
	,Stringkey("source")> CompilerParameters;

static const char* cxxNameGet(long int cxxversion)
	{
	if(cxxversion < 201103L)
		{return "c++03";}
	if(cxxversion < 201402L)
		{return "c++11";}
	return "c++14";
	}

static std::string
cxxVersionString(const char* stdprefix,long int cxxversion)
	{
	std::string ret(stdprefix);
	ret+=cxxNameGet(cxxversion);
	return ret;
	}

long int TargetCxxCompiler::cxxversionDefaultGet() const
	{
	if(m_cxxversion_default==0)
		{
		m_cxxversion_default=::cxxversionDefaultGet(r_options.versionqueryGet()
			,{r_paramset.data(), r_paramset.data() + r_paramset.size()});
		}
	return m_cxxversion_default;
	}

void TargetCxxCompiler::compileObject(const char* source,const char* dest
	,const TargetCxxOptions& options_extra) const
	{
	CompilerParameters cxxparams;
	cxxparams.get<Stringkey("target")>().push_back(dest);
	cxxparams.get<Stringkey("source")>().push_back(source);

	auto options_result=r_options;
//TODO merge with options_extra

	auto cxxversion_min=options_result.cxxversionMinGet();
	if(cxxversion_min > cxxversionDefaultGet())
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_min));
		}

//TODO Fix includedir

	std::vector<const ParameterSet*> params(r_paramset);
	params.push_back(&cxxparams);
	auto compiler=r_options.objcompileGet().execute(Pipe::REDIRECT_STDERR
		,{params.data(),params.data() + params.size()});
	auto stream=compiler.stderrCapture();
	ReadBuffer rb(*stream.get());
	while(!rb.eof())
		{
		auto ch_in=rb.byteRead();
		putchar(ch_in);
		}
	auto res=compiler.exitStatusGet();
	if(res!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
			"The compiler returned status code #2;",{source,dest,res}));
		}
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

void TargetCxxCompiler::compileApplication(Twins<const FileInfo*> files
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	const char* source=files.first[0].filename;

	CompilerParameters cxxparams;
	cxxparams.get<Stringkey("target")>().push_back(dest);

	auto options_result=r_options;
//TODO merge with options_extra
	{
	auto& deps=cxxparams.get<Stringkey("dependencies")>();
	while(files.first!=files.second)
		{
		deps.push_back(makeDepitemString(*files.first,options_result));
		++files.first;
		}
//TODO Fix libdir
	}

	auto cxxversion_min=options_result.cxxversionMinGet();
	if(cxxversion_min >  cxxversionDefaultGet())
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_min));
		}


//TODO Fix includedir
	std::vector<const ParameterSet*> params(r_paramset);
	params.push_back(&cxxparams);
	auto compiler=r_options.appcompileGet().execute(Pipe::REDIRECT_STDERR
		,{params.data(),params.data() + params.size()});
	auto stream=compiler.stderrCapture();
	ReadBuffer rb(*stream.get());
	while(!rb.eof())
		{
		fputc(rb.byteRead(),stderr);
		}
	auto res=compiler.exitStatusGet();
	if(res!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
			"The compiler returned status code #2;",{source,dest,res}));
		}
	}

void TargetCxxCompiler::compileDll(Twins<const FileInfo*> files
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	const char* source=files.first[0].filename;

	CompilerParameters cxxparams;
	cxxparams.get<Stringkey("target")>().push_back(dest);

	auto options_result=r_options;
//TODO merge with options_extra
	{
	auto& deps=cxxparams.get<Stringkey("dependencies")>();
	while(files.first!=files.second)
		{
		deps.push_back(makeDepitemString(*files.first,options_result));
		++files.first;
		}
//TODO Fix libdir
	}

	auto cxxversion_min=options_result.cxxversionMinGet();
	if(cxxversion_min >  cxxversionDefaultGet())
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_min));
		}


//TODO Fix includedir
	std::vector<const ParameterSet*> params(r_paramset);
	params.push_back(&cxxparams);
	auto compiler=r_options.dllcompileGet().execute(Pipe::REDIRECT_STDERR
		,{params.data(),params.data() + params.size()});
	auto stream=compiler.stderrCapture();
	ReadBuffer rb(*stream.get());
	while(!rb.eof())
		{
		fputc(rb.byteRead(),stderr);
		}
	auto res=compiler.exitStatusGet();
	if(res!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
			"The compiler returned status code #2;",{source,dest,res}));
		}
	}
