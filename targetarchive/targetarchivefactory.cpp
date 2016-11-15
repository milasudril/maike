//@	{"targets":[{"name":"targetarchivefactory.o","type":"object"}]}

#include "targetarchivefactory.hpp"

using namespace Maike;

TargetArchiveFactory::TargetArchiveFactory(const TargetArchiveCompiler& intpret):
	r_intpret(intpret)
	{}

Handle<Target> TargetArchiveFactory::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,const char* root
	,size_t id,size_t line_count) const
	{
	return Handle<TargetArchive>( TargetArchive::create(obj,r_intpret,name_src,in_dir,root
		,id,line_count) );
	}
