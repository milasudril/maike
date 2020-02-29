//@	{
//@	 "targets":[{"name":"dependency.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DEPENDENCY_HPP
#define MAIKE_DEPENDENCY_HPP

#include "fs.hpp"

namespace Maike
{
	class SourceFile;

	class Dependency
	{
		public:
			enum class Resolver:int{InternalLookup, None};

			explicit Dependency(fs::path&& path, Resolver res_method):
			m_name{std::move(path)}, m_res_method{res_method}
			{
			}

			fs::path const& name() const
			{
				return r_srcfile != nullptr? r_srcfile.name() : m_name;
			}

			template<class SrcFileSet>
			Dependency& resolve(SrcFileSet const& source_files)
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

			SourceFile const* sourceFile() const
			{
				return r_srcfile;
			}

		private:
			fs::path m_name;
			SourceFile const* r_srcfile;
			Resolver m_res_method;

	};
}

#endif