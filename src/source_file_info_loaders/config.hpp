//@	{
//@	  "targets":[{"name":"config.hpp","type":"include"}]
//@	}

#ifndef MAIKE_SOURCEFILEINFOLOADERS_CONFIG_HPP
#define MAIKE_SOURCEFILEINFOLOADERS_CONFIG_HPP

//#include "./source_file_info_loader.hpp"
#include "src/key_value_store/compound.hpp"

#include <string>
#include <map>

namespace Maike::SourceFileInfoLoaders
{
	class Config
	{
	public:
		Config()
		{
		}

		explicit Config(KeyValueStore::CompoundRefConst)
		{
		}

	private:
		//		std::map<std::string, SourceFileInfoLoader> m_loaders;
	};

	inline auto fromJson(KeyValueStore::Empty<Config>, KeyValueStore::JsonRefConst ref)
	{
		return Config{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Config const&)
	{
		return KeyValueStore::Compound{}.takeHandle();
	}

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