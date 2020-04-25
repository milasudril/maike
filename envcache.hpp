//@	{
//@	  "targets":[{"name":"envcache.hpp","type":"include"}]
//@	  ,"dependencies_extra":[{"ref":"envcache.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_ENVCACHE_HPP
#define MAIKE_ENVCACHE_HPP

#include <map>

namespace Maike
{
	class EnvCache: private std::map<std::string, std::string, std::less<>>
	{
		using Base = std::map<std::string, std::string, std::less<>>;

	public:
		EnvCache()
		{
			reload();
		}

		using Base::begin;
		using Base::end;
		using Base::find;
		using Base::size;

		template<class... Args>
		auto insert(Args&&... item)
		{
			auto ret = Base::insert(std::forward<Args>(item)...);
			if(ret.second) { m_dirty = true; }
			return ret;
		}

		template<class... Args>
		auto insert_or_assign(Args&&... item)
		{
			auto ret = Base::insert_or_assign(std::forward<Args>(item)...);
			if(ret.second) { m_dirty = true; }
			return ret;
		}

		template<class Key>
		auto& operator[](Key&& key)
		{
			m_dirty = true;
			return Base::operator[](std::forward<Key>(key));
		}

		template<class Key>
		void erase(Key&& key)
		{
			auto i = find(std::forward<Key>(key));
			if(i != end())
			{
				Base::erase(i);
				m_dirty = true;
			}
		}

		bool dirty() const
		{
			return m_dirty;
		}

		void reload();

		void updateEnviron();

	private:
		bool m_dirty;
	};

	extern EnvCache env;
}
#endif