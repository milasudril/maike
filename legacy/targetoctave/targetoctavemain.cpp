//@	[
//@		[
//@		"windows",
//@			{"targets":[{"name":"../targetoctave.dll","type":"lib_dynamic"}]}
//@		],[
//@		"os_x",
//@			{"targets":[{"name":"../targetoctave.dylib","type":"lib_dynamic"}]}
//@		],{
//@		"targets":
//@			[{"name":"../targetoctave.so","type":"lib_dynamic"}]
//@		}
//@	]

#include "targetoctavehook.hpp"
#include "../exceptionhandler.hpp"
#include "../stdstream.hpp"

using namespace Maike;

void Maike_Plugin_init(DataSink& standard_output,DataSink& standard_error
	,ExceptionHandler eh)
	{
	exceptionHandlerSet(eh);
	StdStream::outputSet(standard_output);
	StdStream::errorSet(standard_error);
	}

void Maike_Plugin_cleanup()
	{}

Target_Hook* Maike_Target_Hook_create(const ParameterSetDumpable& params)
	{
	return TargetOctaveHook::create(params);
	}
