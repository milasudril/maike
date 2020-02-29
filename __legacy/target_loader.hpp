//@	{"targets":[{"name":"target_loader.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_LOADER_HPP
#define MAIKE_TARGET_LOADER_HPP

#include "visibility.hpp"
#include "resourceobject.hpp"
#include "handle.hpp"

namespace Maike
	{
	class Spider;
	class DependencyGraph;
	class Target_FactoryDelegator;
	class DependencyBuffer;
	class Target;

	class PRIVATE Target_Loader
		{
		public:
			virtual void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const=0;

			virtual void dependenciesExtraGet(const char* name_src,const char* in_dir
				,const char* root,ResourceObject::Reader rc_reader
				,DependencyBuffer& buffer) const=0;

			virtual void dependenciesGet(const char* name_src,const char* in_dir
				,const char* root,ResourceObject::Reader rc_reader
				,DependencyBuffer& deps) const=0;

			virtual Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root
				,size_t id,size_t line_count) const=0;


		protected:
			~Target_Loader()=default;
		};
	}

#endif
