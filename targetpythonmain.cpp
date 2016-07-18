//@	[
//@		[
//@		"windows",
//@			{"targets":[{"name":"targetpython.dll","type":"lib_dynamic"}]}
//@		],[
//@		"os_x",
//@			{"targets":[{"name":"targetpython.dylib","type":"lib_dynamic"}]}
//@		],{
//@		"targets":
//@			[{"name":"targetpython.so","type":"lib_dynamic"}]
//@		}
//@	]

#include "targetpythonhook.hpp"
#include "exceptionhandler.hpp"

using namespace Maike;

Target_Hook* Maike_Target_Hook_create(const ParameterSetDumpable& params
	,DataSink& standard_error
	,ExceptionHandler eh)
	{
	exceptionHandlerSet(eh);
	return TargetPythonHook::create(params);
	}
