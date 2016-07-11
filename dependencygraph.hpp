//@	{
//@	"targets":[{"name":"dependencygraph.hpp","type":"include"}]
//@	}

#ifndef MAIKE_DEPENDENCYGRAPH_HPP
#define MAIKE_DEPENDENCYGRAPH_HPP

#include <cstddef>

namespace Maike
	{
	template<class T>
	class Handle;

	class Target;

	template<class T>
	class Twins;

	class Stringkey;

	class DependencyGraph
		{
		public:
			virtual DependencyGraph& targetRegister(Handle<Target>&& target)=0;
			virtual DependencyGraph& targetsPatch()=0;
			virtual const Twins<size_t>& idRangeGet() const noexcept=0;

			class TargetProcessor
				{
				public:
					virtual ~TargetProcessor()=default;
					virtual int operator()(DependencyGraph& graph,Target& target)=0;
				};

			virtual DependencyGraph& targetsProcess(TargetProcessor&& proc)=0;

			virtual DependencyGraph& targetsRemove(TargetProcessor&& condition)=0;

			virtual Target* targetFind(const Stringkey& key)=0;

			virtual DependencyGraph& clear() noexcept=0;

		protected:
			~DependencyGraph()=default;
		};
	}

#endif
