//@ {
//@ "targets":[ {"name":"target.hpp","type":"include"} ]
//@ }

#ifndef MAIKE_TARGET_HPP
#define MAIKE_TARGET_HPP

#include "twins.hpp"
#include <cstddef>

namespace Maike
	{
	class Dependency;
	class Invoker;

	class Target
		{
		public:
			virtual ~Target()=default;
			virtual Target& childCounterIncrement() noexcept=0;
			virtual size_t childCounterGet() const noexcept=0;
			virtual void compile(Twins<const Dependency*> dependency_list
				,Invoker& invoker)=0;
			virtual Target& dependencyAdd(Dependency&& dep)=0;
			virtual Twins<const Dependency*> dependencies() const noexcept=0;
			virtual Twins<Dependency*> dependencies() noexcept=0;
			virtual size_t idGet() const noexcept=0;
			virtual const char* nameGet() const noexcept=0;
			virtual const char* nameFullGet() const noexcept=0;
			virtual const char* sourceNameGet() const noexcept=0;
			virtual const char* sourceNameFullGet() const noexcept=0;
			virtual bool upToDate(Twins<const Dependency*> dependency_list
				,Invoker& invoker) const=0;
		};
	}

#endif
