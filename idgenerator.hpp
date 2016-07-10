//@	{"targets":[{"name":"idgenerator.hpp","type":"include"}]}

#ifndef MAIKE_IDGENERATOR_HPP
#define MAIKE_IDGENERATOR_HPP

#include <stack>

namespace Maike
	{
	template<class IdType>
	class IdGenerator
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
				auto ret=std::move(m_freelist.top());
				m_freelist.pop();
				return ret;
				}

			void idRelease(const IdType& id)
				{m_freelist.push(id);}

		private:
			std::stack<IdType> m_freelist;
			IdType m_id_next;
		};
	}

#endif
