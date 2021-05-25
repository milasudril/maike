//@	{
//@	 "dependencies_extra":[{"ref":"./compiler.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_COMPILER_HPP
#define MAIKE_DB_COMPILER_HPP

#include "./dependency.hpp"

#include "core/utils/fs.hpp"
#include "core/key_value_store/compound.hpp"

#include <string>

namespace Maike::Db
{
	class Compiler
	{
	public:
		Compiler() = default;

		explicit Compiler(fs::path&& recipe, bool use_get_tags):
		   m_recipe{std::move(recipe)}, m_use_get_gags{use_get_tags}
		{
		}

		explicit Compiler(fs::path&& recipe, KeyValueStore::Compound&& config, bool use_get_tags):
		   m_recipe{std::move(recipe)}, m_config{std::move(config)}, m_use_get_gags{use_get_tags}
		{
		}

		explicit Compiler(fs::path&& recipe,
		                  std::optional<SourceFileOrigin> origin,
		                  KeyValueStore::Compound&& config,
		                  bool use_get_tags):
		   m_recipe{std::move(recipe)},
		   m_origin{origin},
		   m_config{std::move(config)},
		   m_use_get_gags{use_get_tags}
		{
		}

		explicit Compiler(KeyValueStore::CompoundRefConst config):
		   m_recipe{config.get<char const*>("recipe")},
		   m_config{config.get<KeyValueStore::CompoundRefConst>("config")}
		{
		}

		fs::path const& recipe() const
		{
			return m_recipe;
		}

		KeyValueStore::Compound const& config() const
		{
			return m_config;
		}

		std::optional<SourceFileOrigin> origin() const
		{
			return m_origin;
		}

		template<class PathResolver>
		Compiler& resolveRecipe(PathResolver const& resolver) &
		{
			auto res = resolver.get(m_recipe);
			m_recipe = res.first;
			m_origin = res.second;
			configRecipe();
			return *this;
		}

		template<class PathResolver>
		Compiler&& resolveRecipe(PathResolver const& resolver) &&
		{
			return std::move(resolveRecipe(resolver));
		}

		bool useGetTags() const
		{
			return m_use_get_gags;
		}

	private:
		fs::path m_recipe;
		std::optional<SourceFileOrigin> m_origin;
		KeyValueStore::Compound m_config;
		bool m_use_get_gags{false};

		void configRecipe();
	};

	inline auto fromJson(KeyValueStore::Empty<Compiler>, Maike::KeyValueStore::JsonRefConst val)
	{
		auto const obj = val.as<KeyValueStore::CompoundRefConst>();
		auto const recipe = obj.getIf<char const*>("recipe");

		auto use_get_tags = false;
		if(auto val = obj.getIf<json_int_t>("use_get_tags"); val) { use_get_tags = *val != 0; }

		return Compiler{recipe ? *recipe : "",
		                KeyValueStore::Compound{obj.get<KeyValueStore::CompoundRefConst>("config")},
		                use_get_tags};
	}

	inline auto toJson(Compiler const& obj)
	{
		return KeyValueStore::Compound{}
		   .set("recipe", obj.recipe().c_str())
		   .set("config", obj.config())
		   .set("use_get_tags", static_cast<int>(obj.useGetTags()))
		   .takeHandle();
	}

	Compiler operator|(Compiler const& a, Compiler const& b);

	inline Dependency makeDependency(Compiler const& a)
	{
		return Dependency{
		   a.recipe(), a.origin() ? *a.origin() : SourceFileOrigin::System, std::vector<Property>{}};
	}

	KeyValueStore::Compound getTags(Compiler const& compiler, fs::path const& source_file);
}
#endif
