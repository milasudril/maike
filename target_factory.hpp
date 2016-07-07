//@	{"targets":[{"name":"target_factory.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_FACTORY_HPP
#define MAIKE_TARGET_FACTORY_HPP

#include "handle.hpp"

namespace Maike
	{
	class Target;
	class ResourceObject;
	class Target_Factory
		{
		public:
			virtual Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,size_t id) const=0;
		};
	}

#endif
