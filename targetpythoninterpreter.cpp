//@	{"targets":[{"name":"targetpythoninterpreter.o","type":"object"}]}

#include "targetpythoninterpreter.hpp"
#include "resourceobject.hpp"
#include "parametersetmapfixed.hpp"
#include "readbuffer.hpp"
#include <cstdio>

using namespace Maike;

TargetPythonInterpreter::TargetPythonInterpreter(const ResourceObject& obj
	,const ParameterSet& sysvars)
	:m_interpreter(obj.objectGet("command")),r_sysvars(sysvars)
	{
	}

void TargetPythonInterpreter::run(const char* script,Twins<const char* const*> args) const
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

	auto standard_error=pipe.stderrCapture();
	ReadBuffer rb(*standard_error.get());
	while(!rb.eof())
		{
		fputc(rb.byteRead(),stderr);
		}
	}

void TargetPythonInterpreter::configClear()
	{
	m_interpreter.nameSet("python3").argumentsClear().argumentAppend("--")
		.argumentAppend("{script}").argumentAppend("{args}");
	}

TargetPythonInterpreter& TargetPythonInterpreter::configAppend(const ResourceObject& pythonoptions)
	{
	if(pythonoptions.objectExists("command"))
		{m_interpreter=Command(pythonoptions.objectGet("command"));}
	return *this;
	}
