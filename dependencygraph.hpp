//@	{
//@	"targets":[{"name":"dependencygraph.hpp","type":"include"}]
//@	}

#ifndef MAIKE_DEPENDENCYGRAPH_HPP
#define MAIKE_DEPENDENCYGRAPH_HPP

#include <cstddef>

namespace Maike
	{
	struct TargetHandle;
	class Target;

	class DependencyGraph
		{
		public:
			virtual DependencyGraph& targetRegister(TargetHandle&& target)=0;
			virtual DependencyGraph& targetsPatch()=0;
			virtual size_t targetCounterGet() const noexcept=0;

			class TargetProcessor
				{
				public:
					virtual ~TargetProcessor()=default;
					virtual void operator()(DependencyGraph& graph,Target& target)=0;
				};

			virtual DependencyGraph& targetsProcess(TargetProcessor&& proc)=0;

		protected:
			~DependencyGraph()=default;
		};
	}

#endif
