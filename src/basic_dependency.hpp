//@	{
//@	 "targets":[{"name":"basic_dependency.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_BASICDEPENDENCY_HPP
#define MAIKE_BASICDEPENDENCY_HPP

#include "./fs.hpp"

namespace Maike
{
	template<class SrcFile>
	class BasicDependency
	{
	public:
		enum class Resolver : int
		{
			InternalLookup,
			None
		};

		explicit BasicDependency(fs::path const& path, Resolver res_method):
		   m_name{path},
		   r_srcfile{nullptr},
		   m_res_method{res_method}

		{
		}

		fs::path const& name() const
		{
			return m_name;
		}

		template<class SrcFileSet>
		BasicDependency& resolve(SrcFileSet const& source_files)
		{
			switch(m_res_method)
			{
				case Resolver::InternalLookup:
				{
					r_srcfile = source_files.find(m_name);
				}
				break;
				case Resolver::None: break;
			}
			return *this;
		}

		SrcFile const* sourceFile() const
		{
			return r_srcfile;
		}

	private:
		fs::path m_name;
		SrcFile const* r_srcfile;
		Resolver m_res_method;
	};
}

#endif