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

			void dependenciesExtraGet(const char*,const char*
				,const char*,ResourceObject::Reader
				,DependencyBuffer&) const
				{}

			void dependenciesGet(const char*,const char*
				,const char*,ResourceObject::Reader
				,DependencyBuffer&) const
				{}	

		private:
		};
	}

#endif
