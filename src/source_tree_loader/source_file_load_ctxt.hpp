//@	{
//@	"dependencies_extra":[{"ref":"./source_file_load_ctxt.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_SOURCETREELOADER_SOURCEFILELOADCTXT_HPP
#define MAIKE_SOURCETREELOADER_SOURCEFILELOADCTXT_HPP

#include "./command_dictionary.hpp"

#include "src/db/dependency.hpp"
#include "src/db/target_info.hpp"
#include "src/db/source_file_info.hpp"
#include "src/key_value_store/compound.hpp"
#include "src/source_file_info_loaders/loader.hpp"

namespace Maike::SourceTreeLoader
{
	class SourceFileLoadContext
	{
	public:
		explicit SourceFileLoadContext(std::reference_wrapper<fs::path const> src_path,
		                               std::reference_wrapper<fs::path const> source_dir,
		                               std::reference_wrapper<fs::path const> target_dir):
		   m_src_path{src_path},
		   m_source_file_dir{src_path.get().parent_path().lexically_normal()},
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

		fs::path const sourcePath() const
		{
			return m_src_path.get();
		}

	private:
		std::reference_wrapper<fs::path const> m_src_path;
		fs::path m_source_file_dir;
		std::reference_wrapper<fs::path const> m_source_dir;
		std::reference_wrapper<fs::path const> m_target_dir;
	};


	fs::path prependSearchPath(SourceFileLoadContext const& load_ctxt,
	                           fs::path const& src_name,
	                           Db::SourceFileOrigin expected_origin);

	Db::Dependency prependSearchPath(SourceFileLoadContext const& load_ctxt,
	                                 Maike::Db::Dependency const& dependency);

	Db::Dependency getDependency(SourceFileLoadContext const& load_ctxt,
	                             KeyValueStore::CompoundRefConst dep,
	                             Db::SourceFileOrigin default_origin);

	fs::path makeTargetName(SourceFileLoadContext const& load_ctxt, fs::path const& name);

	Db::TargetInfo getTarget(SourceFileLoadContext const& load_ctxt,
	                         KeyValueStore::CompoundRefConst target);

	std::vector<Db::Dependency> prependSearchPath(SourceFileLoadContext const& load_ctxt,
	                                              std::vector<Db::Dependency> const& deps);

	std::vector<Db::TargetInfo> getTargets(SourceFileLoadContext const& load_ctxt,
	                                       KeyValueStore::Compound const& tags);

	std::vector<Db::Dependency> getUseDeps(SourceFileLoadContext const& load_ctxt,
	                                       KeyValueStore::Compound const& tags);

	std::vector<Db::Dependency> getChildTargetUseDeps(SourceFileLoadContext const& load_ctxt,
	                                                  Maike::KeyValueStore::Compound const& tags);

	Db::SourceFileInfo loadSourceFile(SourceFileLoadContext const& load_ctxt,
	                                  std::vector<Db::Dependency>&& builtin_deps,
	                                  SourceFileInfoLoaders::Loader const& loader,
	                                  CommandDictionary const& commands);
}

#endif