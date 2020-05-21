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
					auto i = source_files.find(m_name);
					r_srcfile = i == std::end(source_files) ? nullptr : &i->second;
					if(r_srcfile == nullptr)
					{ throw std::runtime_error{std::string{"Failed to resolve "} + m_name.string()}; }
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

		Resolver resolver() const
		{
			return m_res_method;
		}

	private:
		fs::path m_name;
		SrcFile const* r_srcfile;
		Resolver m_res_method;
	};
}

#endif