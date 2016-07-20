//@	{"targets":[{"name":"target_loader.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_LOADER_HPP
#define MAIKE_TARGET_LOADER_HPP

#include "visibility.hpp"

namespace Maike
	{
	class Spider;
	class DependencyGraph;
	class Target_FactoryDelegator;

	class PRIVATE Target_Loader
		{
		public:
			virtual void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const=0;

		protected:
			~Target_Loader()=default;
		};
	}

#endif
