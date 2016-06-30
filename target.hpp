//@	{
//@	"targets":[ {"name":"target.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_TARGET_HPP
#define MAIKE_TARGET_HPP

#include "twins.hpp"
#include <cstddef>
#include <memory>

namespace Maike
	{
	class Dependency;
	class Invoker;

	class Target
		{
		public:
			virtual Target& childCounterIncrement() noexcept=0;
			virtual size_t childCounterGet() const noexcept=0;
			virtual void compile(Twins<const Dependency*> dependency_list
				,Invoker& invoker,const char* target_dir)=0;
			virtual Target& dependencyAdd(Dependency&& dep)=0;
			virtual Twins<const Dependency*> dependencies() const noexcept=0;
			virtual Twins<Dependency*> dependencies() noexcept=0;
			virtual size_t idGet() const noexcept=0;
			virtual const char* nameGet() const noexcept=0;
			virtual const char* sourceNameGet() const noexcept=0;
			virtual bool upToDate(Twins<const Dependency*> dependency_list
				,Invoker& invoker,const char* target_dir) const=0;

		protected:
			~Target()=default;
		private:
			friend struct TargetHandle;
			virtual void destroy() noexcept=0;
			static void destroy(Target* target) noexcept
				{target->destroy();}
		};

	struct TargetHandle:public std::unique_ptr<Target,void (*)(Target*)>
		{
		TargetHandle(Target* target):
			std::unique_ptr<Target,void(*)(Target*)>(target,Target::destroy)
			{}
		};
	}

#endif
