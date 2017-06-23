//@	{
//@	 "targets":[{"name":"targetarchiveloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchiveloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETARCHIVELOADER_HPP
#define TARGETARCHIVELOADER_HPP

#include "../target_loader.hpp"

namespace Maike
	{
	class PRIVATE TargetArchiveLoader:public Target_Loader
		{
		public:
			TargetArchiveLoader();

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

			void dependenciesExtraGet(const char* name_src,const char* in_dir
				,const char* root,ResourceObject::Reader rc_reader
				,Target& target) const
				{}

		private:
		};
	}

#endif
