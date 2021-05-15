//@	{
//@	 "targets":[{"name":"source_file_loader_delegator.o","type":"object"}]
//@	}

#include "./source_file_loader_delegator.hpp"

#include "./source_file_load_ctxt.hpp"

#include "src/io/fifo.hpp"
#include "src/io/input_file.hpp"
#include "src/io/reader.hpp"

#include <algorithm>
#include <cstring>

namespace
{
	// TODO: These functions should be moved

	Maike::Db::SourceFileInfo loadSourceFile(Maike::fs::path const& src_dir,
	                                         Maike::fs::path const& src_path,
	                                         Maike::fs::path const& target_dir,
	                                         std::vector<Maike::Db::Dependency>&& builtin_deps,
	                                         Maike::SourceFileInfoLoaders::Loader const& loader,
	                                         Maike::SourceTreeLoader::CommandDictionary const&)
	{
		Maike::Io::Fifo<std::byte> src_fifo;
		Maike::Io::Fifo<std::byte> tags_fifo;

		Maike::Io::InputFile input{src_path};
		loader.filterInput(Maike::Io::Reader{input},
		                   Maike::SourceFileInfoLoaders::SourceOutStream{src_fifo},
		                   Maike::SourceFileInfoLoaders::TagsOutStream{tags_fifo});
		tags_fifo.stop();
		src_fifo.stop();

		Maike::SourceTreeLoader::SourceFileLoadContext load_ctxt{src_path.parent_path(), src_dir, target_dir};
		{
			auto deps = prependSearchPath(load_ctxt, loader.getDependencies(Maike::Io::Reader{src_fifo}));
			builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
		}


		auto tags = Maike::KeyValueStore::Compound{Maike::Io::Reader{tags_fifo}, src_path.string()};
		auto targets = getTargets(load_ctxt, tags);
		auto use_deps = getUseDeps(load_ctxt, tags);
		auto child_target_use_deps = getChildTargetUseDeps(load_ctxt, tags);

		std::copy(std::begin(use_deps), std::end(use_deps), std::back_inserter(builtin_deps));

		auto compiler = tags.getIf<Maike::Db::Compiler>("compiler");
		// TODO: If compiler is valid, it should be looked up in command dictionary
		//       and configured
		[&builtin_deps,
		 &child_target_use_deps](Maike::SourceTreeLoader::SourceFileLoadContext const& load_ctxt,
		                         Maike::Db::Compiler const& compiler,
		                         Maike::fs::path const& src_file) {
			auto tags = getTags(compiler, src_file);
			auto use_deps = getUseDeps(load_ctxt, tags);
			auto ctu_deps = getChildTargetUseDeps(load_ctxt, tags);
			std::copy(std::begin(use_deps), std::end(use_deps), std::back_inserter(builtin_deps));
			std::copy(std::begin(ctu_deps), std::end(ctu_deps), std::back_inserter(child_target_use_deps));
			if(compiler.recipe() != "") { builtin_deps.push_back(makeDependency(compiler)); }
		}(load_ctxt, compiler ? (*compiler) : loader.compiler(), src_path);

		auto const rebuild_policy = [](auto const& tags) {
			if(auto rebuild_policy = tags.template getIf<Maike::Db::RebuildPolicy>("rebuild");
			   rebuild_policy)
			{ return *rebuild_policy; }
			return Maike::Db::RebuildPolicy::OnlyIfOutOfDate;
		}(tags);

		return Maike::Db::SourceFileInfo{std::move(targets),
		                                 std::ref(loader.compiler()),
		                                 compiler ? (*compiler) : Maike::Db::Compiler{""},
		                                 Maike::Db::SourceFileOrigin::Project,
		                                 rebuild_policy}
		   .useDeps(std::move(builtin_deps))
		   .childTargetsUseDeps(std::move(child_target_use_deps));
	}
}

Maike::Db::SourceFileInfo Maike::SourceTreeLoader::SourceFileLoaderDelegator::load(
   fs::path const& src_dir, fs::path const& src_path, fs::path const& target_dir) const
{
	std::vector<Db::Dependency> deps;
	if(src_dir != src_path && src_path != m_dir_compiler.get().recipe())
	{
		deps.push_back(Db::Dependency{src_path.parent_path().lexically_normal(),
		                              Db::SourceFileOrigin::Project,
		                              std::vector<Db::Property>{}});
	}

	if(is_directory(src_path))
	{
		std::vector<Db::TargetInfo> targets;

		// In case of root directory, add target dir as target.
		// NOTE: This will work, because targets are stored as a sequence inside SourceFileInfo. That
		//       is, insertion order is preserved.
		if(src_path == src_dir)
		{ targets.push_back(Db::TargetInfo{fs::path{target_dir}, std::vector<Db::Dependency>{}}); }
		targets.push_back(
		   Db::TargetInfo{target_dir / (src_path.lexically_normal()), std::vector<Db::Dependency>{}});

		deps.push_back(makeDependency(m_dir_compiler));
		return Db::SourceFileInfo{std::move(targets),
		                          m_dir_compiler,
		                          Db::Compiler{""},
		                          Db::SourceFileOrigin::Project,
		                          Db::RebuildPolicy::OnlyIfOutOfDate}
		   .useDeps(std::move(deps));
	}

	std::string extension;
	for(auto p = src_path; !p.extension().empty(); p = p.stem())
	{
		extension = std::string{p.extension()} + extension;
	}

	auto i = m_loaders.find(extension);
	if(i == std::end(m_loaders))
	{ return Db::SourceFileInfo{Db::SourceFileOrigin::Project, Db::Compiler{}}; }

	return loadSourceFile(src_dir, src_path, target_dir, std::move(deps), i->second, m_cmds);
}
