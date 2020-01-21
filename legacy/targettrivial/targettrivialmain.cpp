//@	[
//@		[
//@		"windows",
//@			{"targets":[{"name":"../targettrivial.dll","type":"lib_dynamic"}]}
//@		],[
//@		"os_x",
//@			{"targets":[{"name":"../targettrivial.dylib","type":"lib_dynamic"}]}
//@		],{
//@		"targets":
//@			[{"name":"../targettrivial.so","type":"lib_dynamic"}]
//@		}
//@	]

#include "targettrivialhook.hpp"
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

Target_Hook* Maike_Target_Hook_create(const ParameterSetDumpable&)
	{
	return TargetTrivialHook::create();
	}
