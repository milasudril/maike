//@	[
//@		[
//@		"windows",
//@			{"targets":[{"name":"targetcxx.dll","type":"lib_dynamic"}]}
//@		],[
//@		"os_x",
//@			{"targets":[{"name":"targetcxx.dylib","type":"lib_dynamic"}]}
//@		],{
//@		"targets":
//@			[{"name":"targetcxx.so","type":"lib_dynamic"}]
//@		}
//@	]

#include "targetcxxhook.hpp"
#include "exceptionhandler.hpp"
#include "stdstream.hpp"

using namespace Maike;

Target_Hook* Maike_Target_Hook_create(const ParameterSetDumpable& params
	,DataSink& standard_output
	,DataSink& standard_error
	,ExceptionHandler eh)
	{
	exceptionHandlerSet(eh);
	StdStream::outputSet(standard_output);
	StdStream::errorSet(standard_error);
	return TargetCxxHook::create(params);
	}
