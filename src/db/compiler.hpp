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
		explicit Compiler(fs::path&& m_recipe, std::string&& config):
		   m_recipe{std::move(m_recipe)}, m_config{std::move(config)}
		{
		}

		fs::path const& recipe() const
		{
			return m_recipe;
		}

		KeyValueStore::Compound const& config() const
		{
			return m_cfg_2;
		}

	private:
		fs::path m_recipe;
		std::string m_config;
		KeyValueStore::Compound m_cfg_2;
	};
}
#endif