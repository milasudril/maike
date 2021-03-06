//@	{
//@	 "targets":[{"name":"source_file_loader_delegator.o","type":"object"}]
//@	}

#include "./source_file_loader_delegator.hpp"

#include "./source_file_load_ctxt.hpp"

Maike::Db::SourceFileInfo Maike::SourceTreeLoader::SourceFileLoaderDelegator::load(
   fs::path const& src_path, fs::path const& src_dir, fs::path const& target_dir) const
{
	SourceFileLoadContext load_ctxt{src_path, src_dir, target_dir};

	std::vector<Db::Dependency> deps;
	if(src_dir != src_path && src_path != m_dir_compiler.get().recipe())
	{
		deps.push_back(Db::Dependency{
		   load_ctxt.sourceFileDir(), Db::SourceFileOrigin::Project, std::vector<Db::Property>{}});
	}

	if(is_directory(src_path))
	{
		std::vector<Db::TargetInfo> targets{
		   Db::TargetInfo{makeTargetName(load_ctxt, src_path.filename()), std::vector<Db::Dependency>{}}};

		deps.push_back(makeDependency(m_dir_compiler));
		return Db::SourceFileInfo{std::move(targets),
		                          m_dir_compiler,
		                          Db::Compiler{"", false},
		                          Db::SourceFileOrigin::Project,
		                          Db::RebuildPolicy::OnlyIfOutOfDate}
		   .useDeps(std::move(deps));
	}

	auto i = matchLongestExtension(src_path, m_loaders);
	if(i == std::end(m_loaders))
	{ return Db::SourceFileInfo{Db::SourceFileOrigin::Project, Db::Compiler{}}; }

	return loadSourceFile(load_ctxt, std::move(deps), i->second, m_cmds);
}
