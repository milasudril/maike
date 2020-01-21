//@	{"targets":[{"name":"idgenerator.hpp","type":"include"}]}

#ifndef MAIKE_IDGENERATOR_HPP
#define MAIKE_IDGENERATOR_HPP

#include "visibility.hpp"
#include <vector>

namespace Maike
	{
	template<class IdType>
	class PRIVATE IdGenerator
		{
		public:
			IdGenerator() noexcept:m_id_next(-1){}

			IdType idGet() noexcept
				{
				if(m_freelist.empty())
					{
					++m_id_next;
					return m_id_next;
					}
				auto ret=std::move(m_freelist.back());
				m_freelist.pop_back();
				return ret;
				}

			IdGenerator& idRelease(const IdType& id)
				{
				m_freelist.push_back(id);
				return *this;
				}

			IdGenerator& reset() noexcept
				{
				m_freelist.clear();
				m_id_next=-1;
				return *this;
				}

		private:
			std::vector<IdType> m_freelist;
			IdType m_id_next;
		};
	}

#endif
