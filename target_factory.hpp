//@	{"targets":[{"name":"target_factory.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_FACTORY_HPP
#define MAIKE_TARGET_FACTORY_HPP

#include "handle.hpp"
#include "visibility.hpp"

namespace Maike
	{
	class Target;
	class ResourceObject;
	class PRIVATE Target_Factory
		{
		public:
			virtual Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root
				,size_t id,size_t line_count) const=0;
		protected:
			~Target_Factory()=default;
		};
	}

#endif
