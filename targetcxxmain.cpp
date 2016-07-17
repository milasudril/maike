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

using namespace Maike;

Target_Hook* Maike_Target_Hook_create(const ParameterSetDumpable& params)
	{return TargetCxxHook::create(params);}
