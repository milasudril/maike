//@	{
//@	  "targets":[{"name":"config.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"config.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SOURCEFILEINFOLOADERS_CONFIG_HPP
#define MAIKE_SOURCEFILEINFOLOADERS_CONFIG_HPP

#include "./loader.hpp"
#include "src/key_value_store/compound.hpp"

#include <string>
#include <map>

namespace Maike::SourceFileInfoLoaders
{
	class Config
	{
	public:
		Config();
		explicit Config(KeyValueStore::CompoundRefConst);

		std::map<std::string, Loader> const& loaders() const
		{
			return m_loaders;
		}

	private:
		std::map<std::string, Loader> m_loaders;
	};

	inline auto fromJson(KeyValueStore::Empty<Config>, KeyValueStore::JsonRefConst ref)
	{
		return Config{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	KeyValueStore::JsonHandle toJson(Config const&);

	inline bool operator==(Config const&, Config const&)
	{
		return true;
	}

	inline bool operator!=(Config const& a, Config const& b)
	{
		return !(a == b);
	}
}

#endif