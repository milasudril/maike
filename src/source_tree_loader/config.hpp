//@	{
//@	 "targets":[{"name":"config.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"config.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_SOURCETREELOADER_CONFIG_HPP
#define MAIKE_SOURCETREELOADER_CONFIG_HPP

#include "./input_filter.hpp"

#include "src/key_value_store/compound.hpp"

namespace Maike::SourceTreeLoader
{
	class Config
	{
	public:
		Config():
		   m_file_info_loaders{{".hpp", "cxx"}, {".cpp", "cxx"}, {".app.maikerule", "app"}},
		   m_recursive{false}
		{
		}

		explicit Config(KeyValueStore::CompoundRefConst obj);

		InputFilter const& inputFilter() const
		{
			return m_filter;
		}

		Config& inputFilter(InputFilter&& filter)
		{
			m_filter = filter;
			return *this;
		}

		auto const& fileInfoLoaders() const
		{
			return m_file_info_loaders;
		}

		Config& fileInfoLoaders(std::map<std::string, std::string>&& loaders)
		{
			m_file_info_loaders = std::move(loaders);
			return *this;
		}

		bool recursive() const
		{
			return m_recursive;
		}

		Config& recursive(bool val)
		{
			m_recursive = val;
			return *this;
		}

	private:
		InputFilter m_filter;
		std::map<std::string, std::string> m_file_info_loaders;
		bool m_recursive;
	};

	inline bool operator==(Config const& a, Config const& b)
	{
		return a.inputFilter() == b.inputFilter() && a.fileInfoLoaders() == b.fileInfoLoaders()
		       && a.recursive() == b.recursive();
	}

	inline bool operator!=(Config const& a, Config const& b)
	{
		return !(a == b);
	}

	KeyValueStore::JsonHandle toJson(Config const& cfg);

	inline auto fromJson(KeyValueStore::Empty<Config>, KeyValueStore::JsonRefConst ref)
	{
		return Config{ref.as<KeyValueStore::CompoundRefConst>()};
	}
}

#endif