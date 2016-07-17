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

using namespace Maike;

Target_Hook* Maike_Target_Hook_create(const ParameterSetDumpable& params)
	{return TargetPythonHook::create(params);}
