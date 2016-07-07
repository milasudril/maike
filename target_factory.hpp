//@	{"targets":[{"name":"target_factory.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_FACTORY_HPP
#define MAIKE_TARGET_FACTORY_HPP

#include "handle.hpp"

namespace Maike
	{
	class ResourceObject;
	class Target;
	class Target_Factory
		{
		public:
			virtual Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t id) const=0;

			virtual Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir
				,size_t id) const=0;

			class Callback
				{
				public:
					virtual void operator()(const Target_Factory&,Handle<Target>&& target)=0;
				};

			virtual void targetsCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t id,Callback&& cb) const=0;

			virtual void targetsCreate(const ResourceObject& obj,const char* in_dir
				,size_t id,Callback&& cb)=0;
		};
	}

#endif
