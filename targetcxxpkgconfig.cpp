//@	{"targets":[{"name":"targetcxxpkgconfig.o","type":"object"}]}

#include "targetcxxpkgconfig.hpp"
#include "command.hpp"
#include "target.hpp"
#include "stdstream.hpp"
#include "writebuffer.hpp"
#include "parametersetmapfixed.hpp"
#include "readbuffer.hpp"
#include "thread.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "targetcxxoptions.hpp"


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
					{wb.write("Error: ").write(message.messageGet()).write("\n");}
				}

		private:
			DataSource* r_src;
		};
	}

namespace
	{
	class PkgConfigParser
		{
		public:
			PkgConfigParser(DataSource& src,const char* delim_seq):m_rb(src)
				,m_state(0),r_delim_seq(delim_seq)
				{}

			bool itemGet(std::string& item);

		private:
			ReadBuffer m_rb;
			size_t m_state;
			const char* r_delim_seq;
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


typedef ParameterSetMapFixed<
	 Stringkey("action")
	,Stringkey("libname")> PkgConfigParams;

void Maike::pkgconfigAsk(const Command& cmd,const char* libname
	,Target& target,TargetCxxOptions& options_out)
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
