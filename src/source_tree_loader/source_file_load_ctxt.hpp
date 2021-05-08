//@	{
//@	 "targets":[{"name":"source_file_load_ctxt.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"./source_file_load_ctxt.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_SOURCETREELOADER_SOURCEFILELOADCTXT_HPP
#define MAIKE_SOURCETREELOADER_SOURCEFILELOADCTXT_HPP

#include "src/db/dependency.hpp"
#include "src/db/target_info.hpp"
#include "src/key_value_store/compound.hpp"

namespace Maike::SourceTreeLoader
{
	class SourceFileLoadContext
	{
	public:
		explicit SourceFileLoadContext(fs::path&& source_file_dir,
		                               std::reference_wrapper<fs::path const> source_dir,
		                               std::reference_wrapper<fs::path const> target_dir):
		   m_source_file_dir{std::move(source_file_dir)},
		   m_source_dir{source_dir},
		   m_target_dir{target_dir}
		{
		}


		fs::path const& sourceFileDir() const
		{
			return m_source_file_dir;
		}

		fs::path const& targetDir() const
		{
			return m_target_dir.get();
		}

		fs::path const& sourceDir() const
		{
			return m_source_dir.get();
		}

	private:
		fs::path m_source_file_dir;
		std::reference_wrapper<fs::path const> m_source_dir;
		std::reference_wrapper<fs::path const> m_target_dir;
	};

	Db::Dependency prependSearchPath(SourceFileLoadContext const& load_ctxt,
	                                 Maike::Db::Dependency const& dependency);

	Db::Dependency getDependency(SourceFileLoadContext const& load_ctxt,
	                             KeyValueStore::CompoundRefConst dep,
	                             Db::SourceFileOrigin default_origin);

	Db::TargetInfo getTarget(SourceFileLoadContext const& load_ctxt,
	                         KeyValueStore::CompoundRefConst target);

	std::vector<Db::TargetInfo> getTargets(SourceFileLoadContext const& load_ctxt,
	                                       KeyValueStore::Compound const& tags);

	std::vector<Db::Dependency> getUseDeps(SourceFileLoadContext const& load_ctxt,
	                                       KeyValueStore::Compound const& tags);

	std::vector<Db::Dependency> getChildTargetUseDeps(SourceFileLoadContext const& load_ctxt,
	                                                  Maike::KeyValueStore::Compound const& tags);
}

#endif