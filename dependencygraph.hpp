//@ {
//@ "targets":[{"name":"dependencygraph.hpp","type":"include"}]
//@ }

#ifndef MAIKE_DEPENDENCYGRAPH_HPP
#define MAIKE_DEPENDENCYGRAPH_HPP

#include <memory>

namespace Maike
	{
	class Target;

	class DependencyGraph
		{
		public:
			virtual ~DependencyGraph()=default;
			virtual DependencyGraph& targetRegister(std::unique_ptr<Target>&& target)=0;
			virtual DependencyGraph& targetsPatch()=0;
			virtual size_t targetCounterGet() const noexcept=0;

			class TargetProcessor
				{
				public:
					virtual ~TargetProcessor()=default;
					virtual void operator()(DependencyGraph& graph,Target& target)=0;
				};

			virtual DependencyGraph& targetsProcess(TargetProcessor&& proc)=0;
			virtual Target* targetFind(const char* name)=0;
		};
	}

#endif
