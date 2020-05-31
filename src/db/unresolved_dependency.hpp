//@	{
//@	 "targets":[{"name":"unresolved_dependency.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_UNRESOLVEDDEPENDENCY_HPP
#define MAIKE_DB_UNRESOLVEDDEPENDENCY_HPP

#include "src/fs.hpp"

namespace Maike::Db
{
	class UnresolvedDependency
	{
	public:
		enum class Resolver : int
		{
			InternalLookup,
			None
		};

		explicit UnresolvedDependency(fs::path const& path, Resolver res_method):
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

	private:
		fs::path m_name;
		Resolver m_res_method;
	};

	inline bool operator==(UnresolvedDependency const& a, UnresolvedDependency const& b)
	{
		return a.name() == b.name() && a.resolver() == b.resolver();
	}

	inline bool operator!=(UnresolvedDependency const& a, UnresolvedDependency const& b)
	{
		return !(a == b);
	}
}

#endif