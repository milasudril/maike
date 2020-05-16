//@	{
//@	  "targets":[{"name":"env_store.hpp","type":"include"}]
//@	  ,"dependencies_extra":[{"ref":"env_store.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_ENVSTORE_HPP
#define MAIKE_ENVSTORE_HPP

#include "./env.hpp"

#include "src/key_value_store/compound.hpp"

#include <map>
#include <string>

namespace Maike
{
	class EnvStore: private std::map<std::string, std::string, std::less<>>
	{
		using Base = std::map<std::string, std::string, std::less<>>;

	public:
		using Base::begin;
		using Base::end;
		using Base::insert;
		using Base::insert_or_assign;
		using Base::operator[];
		using Base::at;
		using Base::find;
		using Base::key_type;
		using Base::mapped_type;
		using Base::size;
		using Base::value_type;

		EnvStore();

		explicit EnvStore(KeyValueStore::CompoundRefConst obj);

		explicit EnvStore(char const* const* env);

		EnvStore& combine(EnvStore const& other);

		Env::StringPointers makeEnviron() const;

		bool operator==(EnvStore const& other) const
		{
			return static_cast<Base const&>(*this) == other;
		}

		bool operator!=(EnvStore const& other) const
		{
			return !(*this == other);
		}
	};

	KeyValueStore::JsonHandle toJson(EnvStore const& env);

	inline auto fromJson(KeyValueStore::Empty<EnvStore>, KeyValueStore::JsonRefConst ref)
	{
		return EnvStore{ref.as<KeyValueStore::CompoundRefConst>()};
	}
}

#endif