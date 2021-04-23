//@	{
//@	  "targets":[{"name":"compiler.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_COMPILER_HPP
#define MAIKE_DB_COMPILER_HPP

#include "src/utils/fs.hpp"
#include "src/key_value_store/compound.hpp"

#include <string>

namespace Maike::Db
{
	class Compiler
	{
	public:
		explicit Compiler(fs::path&& m_recipe): m_recipe{std::move(m_recipe)}
		{
		}

		explicit Compiler(fs::path&& m_recipe, KeyValueStore::Compound&& config):
		   m_recipe{std::move(m_recipe)}, m_config{std::move(config)}
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

		SourceFileOrigin origin() const
		{
			return m_origin;
		}

		template<class PathResolver>
		Compiler& resolveRecipe(PathResolver const& resolver) &
		{
			auto res = resolver.get(m_recipe);
			m_recipe = res.first;
			m_origin = res.second;
			return *this;
		}

		template<class PathResolver>
		Compiler&& resolveRecipe(PathResolver const& resolver) &&
		{
			return std::move(resolveRecipe(resolver));
		}

	private:
		fs::path m_recipe;
		SourceFileOrigin m_origin;
		KeyValueStore::Compound m_config;
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

	inline Compiler operator|(Compiler const& a, Compiler const& b)
	{
		return Compiler{fs::path{b.recipe() != "" ? b.recipe() : a.recipe()},
		                KeyValueStore::Compound{a.config()} | b.config().reference()};
	}
}
#endif