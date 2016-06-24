//@ {
//@ "targets":[{"name":"dependencygraphdefault.hpp","type":"include"}]
//@ "dependencies_extra":[{"ref":"dependencygraphdefault.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_DEPENDENCYGRAPHDEFAULT_H
#define MAIKE_DEPENDENCYGRAPHDEFAULT_H

#include "dependencygraph.hpp"
#include "stringkey.hpp"
#include <map>

namespace Maike
	{
	class DependencyGraphDefault:public DependencyGraph
		{
		public:
			DependencyGraphDefault& targetRegister(std::unique_ptr<Target>&& target);
			DependencyGraphDefault& targetsPatch();
			DependencyGraphDefault& targetsProcess(TargetProcessor&& visitor);
			Target* targetFind(const char* name);
			size_t targetCounterGet() const noexcept
				{return m_targets.size();}

		private:
			typedef Stringkey KeyType;
			std::map< KeyType,std::unique_ptr<Target> > m_targets;
		};
	}

#endif // MAIKE_DEPENDENCYGRAPHDEFAULT_H
