//@	{
//@	 "targets":[{"name":"targetpythonloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythonloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETPYTHONLOADER_HPP
#define TARGETPYTHONLOADER_HPP

#include "target_loader.hpp"

namespace Maike
	{
	class TargetPythonInterpreter;
	class TargetPythonLoader:public Target_Loader
		{
		public:
			explicit TargetPythonLoader(const TargetPythonInterpreter& intpret);

			TargetPythonLoader(TargetPythonInterpreter&& intpret)=delete;

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

		private:
			const TargetPythonInterpreter& r_intpret;
		};
	}

#endif
