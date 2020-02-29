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
			enum class Resolver:int{InternalLookup, None};

			explicit BasicDependency(fs::path&& path, Resolver res_method):
			m_name{std::move(path)}, m_res_method{res_method}
			{
			}

			fs::path const& name() const
			{
				return r_srcfile == nullptr? m_name : r_srcfile->name();
			}

			template<class SrcFileSet>
			BasicDependency& resolve(SrcFileSet const& source_files)
			{
				switch(m_res_method)
				{
					case Resolver::InternalLookup:
						{
							auto i = source_files.find(m_name);
							if( i != source_files.end())
							{
								r_srcfile = &(*i);
							}
						}
						break;
					case Resolver::None:
						break;
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