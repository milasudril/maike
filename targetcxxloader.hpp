//@	{
//@	 "targets":[{"name":"targetcxxloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXLOADER_HPP
#define MAIKE_TARGETCXXLOADER_HPP

#include "targetloader.hpp"

namespace Maike
	{
	class TargetCxxCompiler;

	class TargetCxxLoader:public TargetLoader
		{
		public:
			TargetCxxLoader(TargetCxxCompiler&& compiler)=delete;

			TargetCxxLoader(const TargetCxxCompiler& compiler);

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,const ExpressionEvaluator& evaluator) const;

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

		private:
			const TargetCxxCompiler& r_compiler;
		};
	}

#endif
