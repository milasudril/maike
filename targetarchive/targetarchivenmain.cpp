//@	[
//@		[
//@		"windows",
//@			{"targets":[{"name":"../targetarchive.dll","type":"lib_dynamic"}]}
//@		],[
//@		"os_x",
//@			{"targets":[{"name":"../targetarchive.dylib","type":"lib_dynamic"}]}
//@		],{
//@		"targets":
//@			[{"name":"../targetarchive.so","type":"lib_dynamic"}]
//@		}
//@	]

#include "targetarchivehook.hpp"
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
	return TargetArchiveHook::create(params);
	}
