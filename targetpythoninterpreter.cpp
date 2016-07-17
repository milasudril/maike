//@	{"targets":[{"name":"targetpythoninterpreter.o","type":"object"}]}

#include "targetpythoninterpreter.hpp"
#include "resourceobject.hpp"
#include "parametersetmapfixed.hpp"
#include "readbuffer.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "thread.hpp"
#include "parametersetdumpable.hpp"

#include "writebuffer.hpp"
#include <cstdio>

using namespace Maike;

TargetPythonInterpreter::TargetPythonInterpreter(const ParameterSetDumpable& sysvars)
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
				try
					{
					ReadBuffer rb(*r_src);
					while(!rb.eof())
						{fputc(rb.byteRead(),stderr);}
					}
				catch(const ErrorMessage& message)
					{fprintf(stderr,"Error: %s\n",message.messageGet());}
				}

		private:
			DataSource* r_src;
		};
	}

static void dataProcess(Pipe& interpreter,const ParameterSetDumpable& sysvars)
	{
	auto standard_error=interpreter.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_input=interpreter.stdinCapture();
	ResourceObject sysvars_out(ResourceObject::Type::OBJECT);
	sysvars.configDump(sysvars_out);
	sysvars_out.write(*standard_input.get());
	}

int TargetPythonInterpreter::run(const char* script,Twins<const char* const*> args) const
	{
	ParameterSetMapFixed<Stringkey("args"),Stringkey("script")> params;
	params.get<Stringkey("script")>().push_back(std::string(script));
	while(args.first!=args.second)
		{
		params.get<Stringkey("args")>().push_back(std::string(*args.first));
		++args.first;
		}

	const ParameterSet* paramset_tot[]={&r_sysvars,&params};
	auto pipe=m_interpreter.execute(Pipe::REDIRECT_STDERR|Pipe::REDIRECT_STDIN
		,{paramset_tot,paramset_tot + 2});

	dataProcess(pipe,r_sysvars);

	auto ret=pipe.exitStatusGet();
	if(ret>1)
		{
		exceptionRaise(ErrorMessage("#0;: Script failed",{script}));
		}
	return ret;
	}

void TargetPythonInterpreter::configClear()
	{
	m_interpreter.nameSet("").argumentsClear();
	}

TargetPythonInterpreter& TargetPythonInterpreter::configAppendDefault()
	{
	m_interpreter.argumentsClear();
	m_interpreter.nameSet("python3").argumentAppend("--")
		.argumentAppend("{script}").argumentAppend("{args}");
	return *this;
	}


TargetPythonInterpreter& TargetPythonInterpreter::configAppend(const ResourceObject& pythonoptions)
	{
	if(pythonoptions.objectExists("command"))
		{m_interpreter=Command(pythonoptions.objectGet("command"));}
	return *this;
	}

void TargetPythonInterpreter::configDump(ResourceObject& pythonoptions) const
	{
	ResourceObject command(ResourceObject::Type::OBJECT);
	m_interpreter.configDump(command);
	pythonoptions.objectSet("command",std::move(command));
	}
