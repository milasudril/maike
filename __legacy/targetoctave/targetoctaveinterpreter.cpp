//@	{"targets":[{"name":"targetoctaveinterpreter.o","type":"object"}]}

#include "targetoctaveinterpreter.hpp"
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

TargetOctaveInterpreter::TargetOctaveInterpreter(const ParameterSetDumpable& sysvars):
   r_sysvars(sysvars)
{
	configClear();
}


namespace
{
	class ReadCallback
	{
	public:
		explicit ReadCallback(DataSource* src): r_src(src)
		{
		}

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
			{
				wb.write("Error: ").write(message.messageGet());
			}
		}

	private:
		DataSource* r_src;
	};
}

static void dataProcess(Pipe& interpreter)
{
	auto standard_error = interpreter.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
}

int TargetOctaveInterpreter::run(const char* script, Twins<const char* const*> args) const
{
	ParameterSetMapFixed<Stringkey("args"), Stringkey("script")> params;
	params.get<Stringkey("script")>().push_back(std::string(script));
	while(args.first != args.second)
	{
		params.get<Stringkey("args")>().push_back(std::string(*args.first));
		++args.first;
	}

	const ParameterSet* paramset_tot[] = {&r_sysvars, &params};
	auto pipe = m_interpreter.execute(Pipe::REDIRECT_STDERR, {paramset_tot, paramset_tot + 2});

	dataProcess(pipe);

	auto ret = pipe.exitStatusGet();
	if(ret.killedWas())
	{
		exceptionRaise(ErrorMessage("#0;: The interpreter was killed with signal #1;.",
		                            {script, ret.errorCodeGet()}));
	}
	if(ret.errorCodeGet() > 1)
	{
		exceptionRaise(
		   ErrorMessage("#0;: Script failed with status code #1;.", {script, ret.errorCodeGet()}));
	}
	return ret.errorCodeGet();
}

void TargetOctaveInterpreter::configClear()
{
	m_interpreter.nameSet("").argumentsClear();
}

TargetOctaveInterpreter& TargetOctaveInterpreter::configAppendDefault()
{
	m_interpreter.argumentsClear();
	m_interpreter.nameSet("octave-cli").argumentAppend("{script}").argumentAppend("{args}");
	return *this;
}


TargetOctaveInterpreter& TargetOctaveInterpreter::configAppend(const ResourceObject& octaveoptions)
{
	if(octaveoptions.objectExists("command"))
	{ m_interpreter = Command(octaveoptions.objectGet("command")); }
	return *this;
}

void TargetOctaveInterpreter::configDump(ResourceObject& octaveoptions) const
{
	auto command = octaveoptions.createObject();
	m_interpreter.configDump(command);
	octaveoptions.objectSet("command", std::move(command));
}
