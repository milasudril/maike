//@ {
//@ "targets":[{"name":"dependencygraph.hpp","type":"include"}
//@ }

#ifndef MAIKE_DEPENDENCYGRAPH_H
#define MAIKE_DEPENDENCYGRAPH_H

#include <memory>

namespace Maike
	{
	class Target;

	class DependencyGraph
		{
		public:
			virtual DependencyGraph& targetRegister(std::unique_ptr<Target>&& target)=0;
			virtual DependencyGraph& targetsPatch()=0;

			class TargetProcessor
				{
				public:
					virtual void visit(DependencyGraph& graph,Target& target)=0;
				};

			virtual void targetsProcess(TargetProcessor&& visitor)=0;
			virtual Target* targetFind(const char* name)=0;
		};
	}

#endif
