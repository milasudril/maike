//@	{"targets":[{"name":"targetarchivecompiler.o","type":"object"}]}

#include "targetarchivecompiler.hpp"
#include "../resourceobject.hpp"
#include "../parametersetmapfixed.hpp"
#include "../readbuffer.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../exceptionhandler.hpp"
#include "../thread.hpp"
#include "../parametersetdumpable.hpp"
#include "../stdstream.hpp"
#include "../writebuffer.hpp"


using namespace Maike;

TargetArchiveCompiler::TargetArchiveCompiler(const ParameterSetDumpable& sysvars)
	:r_sysvars(sysvars)
	{
	configClear();
	}


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

static void dataProcess(Pipe& compiler,const ParameterSetDumpable& sysvars)
	{
	auto standard_error=compiler.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_input=compiler.stdinCapture();
	sysvars.configDump().write(*standard_input.get());
	}

int TargetArchiveCompiler::run(const char* script,Twins<const char* const*> args) const
	{
	ParameterSetMapFixed<Stringkey("args"),Stringkey("script")> params;
	params.get<Stringkey("script")>().push_back(std::string(script));
	while(args.first!=args.second)
		{
		params.get<Stringkey("args")>().push_back(std::string(*args.first));
		++args.first;
		}

	const ParameterSet* paramset_tot[]={&r_sysvars,&params};
	auto pipe=m_compiler.execute(Pipe::REDIRECT_STDERR|Pipe::REDIRECT_STDIN
		,{paramset_tot,paramset_tot + 2});

	dataProcess(pipe,r_sysvars);

	auto ret=pipe.exitStatusGet();
	if(ret>1)
		{
		exceptionRaise(ErrorMessage("#0;: Script failed",{script}));
		}
	return ret;
	}

void TargetArchiveCompiler::configClear()
	{
	m_compiler.nameSet("").argumentsClear();
	}

TargetArchiveCompiler& TargetArchiveCompiler::configAppendDefault()
	{
	m_compiler.argumentsClear();
	m_compiler.nameSet("archive3").argumentAppend("--")
		.argumentAppend("{script}").argumentAppend("{args}");
	return *this;
	}


TargetArchiveCompiler& TargetArchiveCompiler::configAppend(const ResourceObject& archiveoptions)
	{
	if(archiveoptions.objectExists("command"))
		{m_compiler=Command(archiveoptions.objectGet("command"));}
	return *this;
	}

void TargetArchiveCompiler::configDump(ResourceObject& archiveoptions) const
	{
	auto command=archiveoptions.createObject();
	m_compiler.configDump(command);
	archiveoptions.objectSet("command",std::move(command));
	}
