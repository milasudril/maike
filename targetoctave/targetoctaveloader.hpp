//@	{
//@	 "targets":[{"name":"targetoctaveloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctaveloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETOCTAVELOADER_HPP
#define TARGETOCTAVELOADER_HPP

#include "../target_loader.hpp"

namespace Maike
	{
	class PRIVATE TargetOctaveLoader:public Target_Loader
		{
		public:
			TargetOctaveLoader();

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

		private:
		};
	}

#endif
