//@	{"targets":[{"name":"targetpythoninterpreter.o","type":"object"}]}

#include "targetpythoninterpreter.hpp"
#include "resourceobject.hpp"
#include "parametersetmapfixed.hpp"
#include "readbuffer.hpp"
#include <cstdio>

using namespace Maike;

TargetPythonInterpreter::TargetPythonInterpreter(const ResourceObject& obj)
	:m_interpreter(obj.objectGet("command"))
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

	ParameterSet* paramset_tot[]={&params};
	auto pipe=m_interpreter.execute(Pipe::REDIRECT_STDERR,{paramset_tot,paramset_tot + 1});

	auto standard_error=pipe.stderrCapture();
	ReadBuffer rb(*standard_error.get());
	while(!rb.eof())
		{
		fputc(rb.byteRead(),stderr);
		}
	}
