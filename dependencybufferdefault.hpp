//@	{"targets":[{"name":"dependencybufferdefault.hpp","type":"include"}]}

#ifndef MAIKE_DEPENDENCYBUFFERDEFAULT_HPP
#define MAIKE_DEPENDENCYBUFFERDEFAULT_HPP

#include "dependencybuffer.hpp"
#include "dependency.hpp"
#include <vector>
#include <algorithm>

namespace Maike
	{
	class Dependency;
	class PRIVATE DependencyBufferDefault final:public DependencyBuffer
		{
		public:
			void append(Dependency&& dep) override
				{m_deps.push_back(std::move(dep));}

			const Dependency* begin() const noexcept override
				{return m_deps.data();}

			const Dependency* end() const noexcept override
				{return begin() + m_deps.size();}

			void append(const DependencyBuffer& buffer) override
				{
				std::for_each(buffer.begin(),buffer.end()
					,[this](const Dependency& dep)
						{
						Dependency cp(dep);
						m_deps.push_back(std::move(cp));
						});
				}

		private:
			std::vector<Dependency> m_deps;
		};
	}

#endif
