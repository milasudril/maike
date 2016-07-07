//@	{
//@	 "targets":[{"name":"targetpythonloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythonloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETPYTHONLOADER_HPP
#define TARGETPYTHONLOADER_HPP

#include "targetloader.hpp"

namespace Maike
	{
	class TargetPythonInterpreter;
	class TargetPythonLoader:public TargetLoader
		{
		public:
			explicit TargetPythonLoader(const TargetPythonInterpreter& intpret);

			TargetPythonLoader(TargetPythonInterpreter&& intpret)=delete;

			virtual void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,const ExpressionEvaluator& evaluator) const;

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

		private:
			const TargetPythonInterpreter& r_intpret;
		};
	}

#endif
