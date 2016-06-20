//@ {
//@ "targets":[{"name":"dependencygraphdefault.hpp","type":"include"}]
//@ "dependencies_extra":[{"ref":"dependencygraphdefault.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_DEPENDENCYGRAPHDEFAULT_H
#define MAIKE_DEPENDENCYGRAPHDEFAULT_H

#include "dependencygraph.hpp"
#include <map>
#include <string>

namespace Maike
	{
	class DependencyGraphDefault : public DependencyGraph
		{
		public:
			virtual DependencyGraphDefault& targetRegister(std::unique_ptr<Target>&& target);
			virtual DependencyGraphDefault& targetsPatch();
			virtual void targetsProcess(TargetProcessor&& visitor);
			virtual Target* targetFind(const char* name);

		protected:

		private:
			typedef std::string KeyType;
			std::map< KeyType,std::unique_ptr<Target> > m_targets;
		};
	}

#endif // MAIKE_DEPENDENCYGRAPHDEFAULT_H
