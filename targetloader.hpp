//@ {
//@ "targets":[{"name":"targetloader.hpp","type":"include"}]
//@ }

#ifndef MAIKE_TARGETLOADER_HPP
#define MAIKE_TARGETLOADER_HPP

namespace Maike
	{
	class Spider;
	class DependencyGraph;

	class TargetLoader
		{
		public:
			virtual void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph) const=0;
		};
	}

#endif
