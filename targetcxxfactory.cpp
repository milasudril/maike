//@	{"targets":[{"name":"targetcxxfactory.o","type":"object"}]}

#include "targetcxxfactory.hpp"

using namespace Maike;

TargetCxxFactory::TargetCxxFactory(const TargetCxxCompiler& compiler):r_compiler(compiler)
	{}

Handle<Target> TargetCxxFactory::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,size_t id) const
	{return Handle<TargetCxx>( TargetCxx::create(obj,r_compiler,name_src,in_dir,id) );}
