//@	{
//@	 "targets":[{"name":"dependency.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_DEPENDENCY_HPP
#define MAIKE_DB_DEPENDENCY_HPP

#include "./source_file_id.hpp"

#include "src/fs.hpp"

namespace Maike::Db
{
	class Dependency
	{
	public:
		enum class Resolver : int
		{
			InternalLookup,
			None
		};

		explicit Dependency(fs::path const& path, Resolver res_method):
		   m_name{path},
		   m_res_method{res_method}
		{
		}

		fs::path const& name() const
		{
			return m_name;
		}

		Resolver resolver() const
		{
			return m_res_method;
		}

		SourceFileId reference() const
		{
			return m_ref;
		}

	private:
		SourceFileId m_ref;
		fs::path m_name;
		Resolver m_res_method;
	};

	inline auto reference(Dependency const& dep)
	{
		return dep.reference();
	}

	inline bool operator==(Dependency const& a, Dependency const& b)
	{
		return a.name() == b.name() && a.resolver() == b.resolver();
	}

	inline bool operator!=(Dependency const& a, Dependency const& b)
	{
		return !(a == b);
	}
}

#endif