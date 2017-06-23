//@	{
//@	 "targets":[{"name":"targetpythonloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythonloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETPYTHONLOADER_HPP
#define TARGETPYTHONLOADER_HPP

#include "../target_loader.hpp"

namespace Maike
	{
	class PRIVATE TargetPythonLoader:public Target_Loader
		{
		public:
			TargetPythonLoader();

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

			void dependenciesExtraGet(const char* name_src,const char* in_dir
				,const char* root,ResourceObject::Reader rc_reader
				,DependencyBuffer& deps) const
				{}	

		private:
		};
	}

#endif
