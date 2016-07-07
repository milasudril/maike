//@	{"targets":[{"name":"targetpythonfactory.o","type":"object"}]}

#include "targetpythonfactory.hpp"

using namespace Maike;

TargetPythonFactory::TargetPythonFactory(const TargetPythonInterpreter& intpret):
	r_intpret(intpret)
	{}

Handle<Target> TargetPythonFactory::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,size_t id) const
	{return Handle<TargetPython>( TargetPython::create(obj,r_intpret,name_src,in_dir,id) );}
