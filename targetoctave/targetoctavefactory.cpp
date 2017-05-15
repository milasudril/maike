//@	{"targets":[{"name":"targetoctavefactory.o","type":"object"}]}

#include "targetoctavefactory.hpp"

using namespace Maike;

TargetOctaveFactory::TargetOctaveFactory(const TargetOctaveInterpreter& intpret):
	r_intpret(intpret)
	{}

Handle<Target> TargetOctaveFactory::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root
	,size_t id,size_t line_count) const
	{
	return Handle<TargetOctave>( TargetOctave::create(obj,r_intpret,name_src,in_dir,root
		,id,line_count) );
	}
