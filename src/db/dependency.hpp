//@	{
//@	 "targets":[{"name":"dependency.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_DEPENDENCY_HPP
#define MAIKE_DB_DEPENDENCY_HPP

#include "./source_file_id.hpp"
#include "./source_file_origin.hpp"

#include "src/fs.hpp"

namespace Maike::Db
{
	class Dependency
	{
	public:
		explicit Dependency(fs::path const& path, SourceFileOrigin expected_origin):
		   m_name{path}, m_expected_origin{expected_origin}
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

	private:
		SourceFileId m_ref;
		fs::path m_name;
		SourceFileOrigin m_expected_origin;
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
}

#endif