//@	{
//@	 "targets":[{"name":"dependencygraphdefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"dependencygraphdefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DEPENDENCYGRAPHDEFAULT_HPP
#define MAIKE_DEPENDENCYGRAPHDEFAULT_HPP

#include "dependencygraph.hpp"
#include "stringkey.hpp"
#include <map>

namespace Maike
	{
	class DependencyGraphDefault:public DependencyGraph
		{
		public:
			DependencyGraphDefault& targetRegister(TargetHandle&& target);
			DependencyGraphDefault& targetsPatch();
			DependencyGraphDefault& targetsProcess(TargetProcessor&& visitor);
			Target* targetFind(const Stringkey& key);
			size_t targetCounterGet() const noexcept
				{return m_targets.size();}

		private:
			std::map< Stringkey,TargetHandle > m_targets;
		};
	}

#endif
