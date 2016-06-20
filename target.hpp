//@ {
//@ "target":{"name":"target.hpp","type":"include"}
//@ }

#ifndef MAIKE_TARGET_H
#define MAIKE_TARGET_H

#include <utility>

namespace Maike
	{
	class Dependency;

	class Target
		{
		public:
			virtual ~Target()=default;
			virtual bool compile(std::pair<Target*> targets_rel) const noexcept=0;
			virtual void dependencyAdd(Dependency&& dep)=0;
			virtual std::pair<const Dependency*> dependenciesGet() const noexcept=0;
			virtual size_t idGet() const noexcept=0;
			virtual const char* nameGet() const noexcept=0;
			virtual const char* nameFullGet() const noexcept=0;
			virtual const char* sourceNameGet() const noexcept=0;
			virtual const char* sourceNameFullGet() const noexcept=0;
			virtual bool leafIs() const noexcept=0;
			virtual bool upToDate(std::pair<Target*> targets_rel) const noexcept=0;
		};
	}

#endif
