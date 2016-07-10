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

extern "C"
	{
	Target_Hook* Maike_Target_Hook_create(const ResourceObject& cxxoptions
		,const ParameterSet& params)
		{return TargetCxxHook::create(cxxoptions,params);}
	}
