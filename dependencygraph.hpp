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
					virtual int operator()(DependencyGraph& graph,Target& target)=0;
				protected:
					~TargetProcessor()=default;
				};

			virtual DependencyGraph& targetsProcess(TargetProcessor&& proc)=0;

			virtual DependencyGraph& targetsRemove(TargetProcessor&& condition)=0;

			class TargetProcessorConst
				{
				public:
					virtual int operator()(const DependencyGraph& graph,const Target& target)=0;
				protected:
					~TargetProcessorConst()=default;
				};

			virtual void targetsProcess(TargetProcessorConst&& proc) const=0;

			virtual Target* targetFind(const Stringkey& key)=0;

			virtual const Target* targetFind(const Stringkey& key) const=0;

			virtual DependencyGraph& clear() noexcept=0;

		protected:
			~DependencyGraph()=default;
		};
	}

#endif
