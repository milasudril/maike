//@	{
//@	  "targets":[{"name":"compiler.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"./compiler.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_COMPILER_HPP
#define MAIKE_DB_COMPILER_HPP

#include "./dependency.hpp"

#include "src/utils/fs.hpp"
#include "src/key_value_store/compound.hpp"

#include <string>

namespace Maike::Db
{
	class Compiler
	{
	public:
		Compiler() = default;

		explicit Compiler(fs::path&& recipe): m_recipe{std::move(recipe)}
		{
		}

		explicit Compiler(fs::path&& recipe, KeyValueStore::Compound&& config):
		   m_recipe{std::move(recipe)}, m_config{std::move(config)}
		{
		}

		explicit Compiler(fs::path&& recipe,
		                  std::optional<SourceFileOrigin> origin,
		                  KeyValueStore::Compound&& config):
		   m_recipe{std::move(recipe)}, m_origin{origin}, m_config{std::move(config)}
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

	private:
		fs::path m_recipe;
		std::optional<SourceFileOrigin> m_origin;
		KeyValueStore::Compound m_config;

		void configRecipe();
	};

	inline auto fromJson(KeyValueStore::Empty<Compiler>, Maike::KeyValueStore::JsonRefConst val)
	{
		auto obj = val.as<KeyValueStore::CompoundRefConst>();
		auto recipe = obj.getIf<char const*>("recipe");
		return Compiler{recipe ? *recipe : "",
		                KeyValueStore::Compound{obj.get<KeyValueStore::CompoundRefConst>("config")}};
	}

	inline auto toJson(Compiler const& obj)
	{
		return KeyValueStore::Compound{}
		   .set("recipe", obj.recipe().c_str())
		   .set("config", obj.config())
		   .takeHandle();
	}

	Compiler operator|(Compiler const& a, Compiler const& b);

	inline Dependency makeDependency(Compiler const& a)
	{
		return Dependency{a.recipe(), a.origin() ? *a.origin() : SourceFileOrigin::System};
	}

	KeyValueStore::Compound getTags(Compiler const& compiler, fs::path const& source_file);
}
#endif