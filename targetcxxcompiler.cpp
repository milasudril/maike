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
	,const ParameterSet& sysvars):r_options(options)
	{
	r_paramset.push_back(&sysvars);
	r_paramset.push_back(&options.paramsGet());
	m_cxxversion_default=cxxversionDefaultGet(options.versionqueryGet()
		,{r_paramset.data(), r_paramset.data() + r_paramset.size()});
	}

// 16f3e46051eee3e8 target
// 477c58df992a139e cxxversion
// 76dbdc228f782db8 source
// 89804aa1f3b7ef83 libdir
// 90999e2acd53f306 includedir
// ccfa12bb6c45a400 dependencies

// dce2956d43782dcc bad_key

typedef ParameterSetMapFixed<Stringkey("cxxversion"),Stringkey("dependencies")
	,Stringkey("includedir"),Stringkey("libdir"),Stringkey("target")
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

void TargetCxxCompiler::compileObject(const char* source,const char* dest
	,const TargetCxxOptions& options_extra) const
	{
	CompilerParameters cxxparams;
	cxxparams.get<Stringkey("target")>().push_back(dest);
	cxxparams.get<Stringkey("source")>().push_back(source);

	auto options_result=r_options;
//TODO merge with options_extra

	auto cxxversion_min=options_result.cxxversionMinGet();
	if(cxxversion_min > m_cxxversion_default)
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

void TargetCxxCompiler::compileApplication(Twins<const char* const*> files
	,const char* dest,const TargetCxxOptions& options_extra) const
	{
	const char* source=files.first[0];

	CompilerParameters cxxparams;
	cxxparams.get<Stringkey("target")>().push_back(dest);
	auto& deps=cxxparams.get<Stringkey("dependencies")>();
	while(files.first!=files.second)
		{
		deps.push_back(std::string(*files.first));
		++files.first;
		}

	auto options_result=r_options;
//TODO merge with options_extra

	auto cxxversion_min=options_result.cxxversionMinGet();
	if(cxxversion_min >  m_cxxversion_default)
		{
		cxxparams.get<Stringkey("cxxversion")>()
			.push_back(cxxVersionString(options_result.stdprefixGet(),cxxversion_min));
		}

//TODO Fix includedir
//TODO Fix libdir

	std::vector<const ParameterSet*> params(r_paramset);
	params.push_back(&cxxparams);
	auto compiler=r_options.appcompileGet().execute(Pipe::REDIRECT_STDERR
		,{params.data(),params.data() + params.size()});
	auto stream=compiler.stderrCapture();
	ReadBuffer rb(*stream.get());
	while(!rb.eof())
		{
		rb.byteRead();
	//	fputc(ch_in,stderr);
		}
	auto res=compiler.exitStatusGet();
	if(res!=0)
		{
		exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
			"The compiler returned status code #2;",{source,dest,res}));
		}
	}
