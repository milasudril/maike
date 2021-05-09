//@	{
//@	 "dependencies_extra":[{"ref":"./dependency.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_DB_DEPENDENCY_HPP
#define MAIKE_DB_DEPENDENCY_HPP

#include "./source_file_id.hpp"
#include "./source_file_origin.hpp"
#include "./property.hpp"

#include "src/utils/fs.hpp"
#include "src/key_value_store/json_handle.hpp"

#include <vector>

namespace Maike::Db
{
	class Dependency
	{
	public:
		explicit Dependency(fs::path const& path,
		                    SourceFileOrigin expected_origin,
		                    std::vector<Property>&& properties):
		   m_name{path}, m_expected_origin{expected_origin}, m_properties{std::move(properties)}
		{
		}

		fs::path const& name() const
		{
			return m_name;
		}

		SourceFileOrigin expectedOrigin() const
		{
			return m_expected_origin;
		}

		SourceFileId reference() const
		{
			return m_ref;
		}

		Dependency& reference(SourceFileId id)
		{
			m_ref = id;
			return *this;
		}

		Dependency& properties(std::vector<Property>&& props)
		{
			m_properties = std::move(props);
			return *this;
		}

		std::vector<Property> const& properties() const
		{
			return m_properties;
		}

	private:
		SourceFileId m_ref;
		fs::path m_name;
		SourceFileOrigin m_expected_origin;
		std::vector<Property> m_properties;
	};

	inline auto reference(Dependency const& dep)
	{
		return dep.reference();
	}

	inline bool operator==(Dependency const& a, Dependency const& b)
	{
		return a.name() == b.name() && a.expectedOrigin() == b.expectedOrigin();
	}

	inline bool operator!=(Dependency const& a, Dependency const& b)
	{
		return !(a == b);
	}

	inline bool isExternal(Dependency const& a)
	{
		return isExternal(a.expectedOrigin());
	}

	namespace detail
	{
		inline bool call_is_older(fs::path const& a, fs::path const& b)
		{
			return isOlder(a, b);
		}
	}

	inline bool isOlder(Dependency const& a, fs::path const& file)
	{
		if(isExternal(a.expectedOrigin()) || is_directory(a.name())) { return true; }

		return detail::call_is_older(a.name(), file);
	}

	KeyValueStore::JsonHandle toJson(Dependency const& dep);
}

#endif
