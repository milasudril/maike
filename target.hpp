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
	class ResourceObject;

	template<class T>
	class Handle;

	class Target
		{
		public:
			typedef Target Base;
			static void destroy(Target* target) noexcept
				{target->destroy();}

			virtual Target& childCounterIncrement() noexcept=0;
			virtual size_t childCounterGet() const noexcept=0;
			virtual void compile(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir)=0;
			virtual Target& dependencyAdd(Dependency&& dep)=0;
			virtual Twins<const Dependency*> dependencies() const noexcept=0;
			virtual Twins<Dependency*> dependencies() noexcept=0;
			virtual size_t idGet() const noexcept=0;
			virtual const char* nameGet() const noexcept=0;
			virtual const char* sourceNameGet() const noexcept=0;
			virtual const char* inDirGet() const noexcept=0;
			virtual bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const=0;
			virtual void dump(ResourceObject& target) const=0;
			virtual size_t lineCountGet() const noexcept=0;
			virtual double compilationTimeGet() const noexcept=0;

		protected:
			~Target()=default;
		private:
			virtual void destroy() noexcept=0;
		};
	}

#endif
