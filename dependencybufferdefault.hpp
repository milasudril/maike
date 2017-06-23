//@	{"targets":[{"name":"dependencybufferdefault.hpp","type":"include"}]}

#ifndef MAIKE_DEPENDENCYBUFFERDEFAULT_HPP
#define MAIKE_DEPENDENCYBUFFERDEFAULT_HPP

#include "dependencybuffer.hpp"
#include "dependency.hpp"
#include <vector>

namespace Maike
	{
	class Dependency;
	class PRIVATE DependencyBufferDefault:public DependencyBuffer
		{
		public:
			void append(Dependency&& dep)
				{m_deps.push_back(std::move(dep));}

			const Dependency* begin() const noexcept
				{return m_deps.data();}

			const Dependency* end() const noexcept
				{return begin() + m_deps.size();}

		private:
			std::vector<Dependency> m_deps;
		};
	}

#endif
