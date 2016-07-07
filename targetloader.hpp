//@	{
//@	"targets":[{"name":"targetloader.hpp","type":"include"}]
//@	}

#ifndef MAIKE_TARGETLOADER_HPP
#define MAIKE_TARGETLOADER_HPP

namespace Maike
	{
	class Spider;
	class DependencyGraph;
	class ExpressionEvaluator;
	class Target_FactoryDelegator;

	class TargetLoader
		{
		public:
			virtual void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,const ExpressionEvaluator& evaluator) const=0;

			virtual void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const=0;

		protected:
			~TargetLoader()=default;
		};
	}

#endif
