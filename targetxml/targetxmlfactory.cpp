//@	{"targets":[{"name":"targetxmlfactory.o","type":"object"}]}

#include "targetxmlfactory.hpp"

using namespace Maike;

Handle<Target> TargetXMLFactory::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root
	,size_t id,size_t line_count) const
	{
	return Handle<TargetXML>( TargetXML::create(obj,name_src,in_dir,root
		,id,line_count) );
	}
