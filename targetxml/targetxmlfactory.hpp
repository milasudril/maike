//@	{
//@	 "targets":[{"name":"targetxmlfactory.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetxmlfactory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETXMLFACTORY_HPP
#define MAIKE_TARGETXMLFACTORY_HPP

#include "../target_factory.hpp"
#include "targetxml.hpp"

namespace Maike
	{
	class PRIVATE TargetXMLFactory:public Target_Factory
		{
		public:
			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root	
				,size_t id,size_t line_count) const;
		};
	}

#endif
