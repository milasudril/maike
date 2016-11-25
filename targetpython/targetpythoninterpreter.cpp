//@	{"targets":[{"name":"targetpythoninterpreter.o","type":"object"}]}

#include "targetpythoninterpreter.hpp"
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
				WriteBuffer wb(StdStream::error());
				try
					{
					ReadBuffer rb(*r_src);
					while(!rb.eof())
						{wb.write(rb.byteRead());}
					}
				catch(const ErrorMessage& message)
					{wb.write("Error: ").write(message.messageGet());}
				}

		private:
			DataSource* r_src;
		};
	}

static void dataProcess(Pipe& interpreter,const ParameterSetDumpable& sysvars)
	{
	auto standard_error=interpreter.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
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
	auto pipe=m_interpreter.execute(Pipe::REDIRECT_STDERR,{paramset_tot,paramset_tot + 2});

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
	auto command=pythonoptions.createObject();
	m_interpreter.configDump(command);
	pythonoptions.objectSet("command",std::move(command));
	}
