//@	{
//@	 "targets":[{"name":"source_file_loader_delegator.o","type":"object"}]
//@	}

#include "./source_file_loader_delegator.hpp"

#include "src/fifo.hpp"
#include "src/input_file.hpp"
#include "src/reader.hpp"
#include "src/key_value_store/array.hpp"
#include "src/key_value_store/compound.hpp"

#include <algorithm>
#include <cstring>

namespace
{
	// TODO: These functions should be moved

	class MkDir
	{
	public:
		int run(Maike::fs::path const&,
		        std::vector<Maike::fs::path const*> const&,
		        std::vector<Maike::fs::path const*> const&,
		        Maike::CompilationLog&) const
		{
			return 0;
		}

		Maike::KeyValueStore::Compound settings() const
		{
			return Maike::KeyValueStore::Compound{};
		}

		void settings(Maike::KeyValueStore::Compound&&)
		{
		}
	};

	std::vector<Maike::Dependency> fixNames(Maike::fs::path const& prefix,
	                                        std::vector<Maike::Dependency> const& deps)
	{
		std::vector<Maike::Dependency> ret;
		ret.reserve(deps.size());
		std::transform(
		   std::begin(deps), std::end(deps), std::back_inserter(ret), [&prefix](auto const& item) {
			   if(item.resolver() == Maike::Dependency::Resolver::InternalLookup)
			   {
				   auto const& str = item.name().string();
				   if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
				   { return Maike::Dependency{(prefix / item.name()).lexically_normal(), item.resolver()}; }
			   }
			   return item;
		   });
		return ret;
	}

	std::vector<Maike::fs::path> getTargets(Maike::fs::path const& src_dir,
	                                        Maike::KeyValueStore::Compound const& tags)
	{
		std::vector<Maike::fs::path> ret;
		if(auto target = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("target"); target)
		{
			auto const name = target->template get<char const*>("name");
			ret.push_back(name);
		}

		if(auto targets = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("targets"); targets)
		{
			std::transform(
			   std::begin(*targets), std::end(*targets), std::back_inserter(ret), [&src_dir](auto item) {
				   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				   return src_dir / Maike::fs::path{val.template get<char const*>("name")};
			   });
		}
		return ret;
	}

	Maike::SourceFileInfo loadSourceFile(std::vector<Maike::Dependency>&& builtin_deps,
	                                     Maike::fs::path const& path,
	                                     Maike::SourceFileLoader const& loader)
	{
		Maike::Fifo<std::byte> src_fifo;
		Maike::Fifo<std::byte> tags_fifo;

		Maike::InputFile input{path};
		loader.filterInput(
		   Maike::Reader{input}, Maike::SourceOutStream{src_fifo}, Maike::TagsOutStream{tags_fifo});
		tags_fifo.stop();
		src_fifo.stop();

		{
			auto deps = fixNames(path.parent_path(), loader.getDependencies(Maike::Reader{src_fifo}));
			builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
		}

		auto tags = Maike::KeyValueStore::Compound{Maike::Reader{tags_fifo}, path.string()};
		auto targets = getTargets(path.parent_path(), tags);

		auto compiler = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("compiler");
		return Maike::SourceFileInfo{std::move(builtin_deps),
		                             std::move(targets),
		                             compiler ? loader.getCompiler(*compiler) : loader.getCompiler()};
	}
}

std::optional<Maike::SourceFileInfo>
Maike::SourceFileLoaderDelegator::load(Maike::fs::path const& path) const
{
	std::vector<Maike::Dependency> deps;
	if(!path.parent_path().empty())
	{
		deps.push_back(Maike::Dependency{path.parent_path().lexically_normal(),
		                                 Maike::Dependency::Resolver::InternalLookup});
	}

	if(is_directory(path))
	{
		std::vector<Maike::fs::path> targets;
		targets.push_back(path.lexically_normal());
		return Maike::SourceFileInfo{std::move(deps), std::move(targets), Maike::Compiler{MkDir{}}};
	}

	auto extension = path.extension();
	auto i = m_loaders.find(extension);
	if(i == std::end(m_loaders)) { return std::optional<Maike::SourceFileInfo>{}; }

	return loadSourceFile(std::move(deps), path, i->second);
}