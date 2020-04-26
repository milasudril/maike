//@	{
//@	  "targets":[{"name":"env_store.o","type":"object"}]
//@	}

#include "./env_store.hpp"

#include "./env.hpp"

Maike::EnvStore::EnvStore():Base
{
	 {"AR", "ar"}
	,{"AS", "as"}
	,{"CC", "cc"}
	,{"CXX", "g++"}
	,{"LD", "ld"}
}
{
}

Maike::EnvStore::EnvStore(KeyValueStore::CompoundRefConst obj)
{
	std::for_each(std::begin(obj), std::end(obj), [this](auto const& item){
		insert_or_assign(item.first, item.second.template as<char const*>());
	});
}

Maike::EnvStore& Maike::EnvStore::loadEnviron(char const* const* env)
{
	auto tmp = Env::load<Base>(env);
	std::for_each(std::begin(tmp), std::end(tmp), [this](auto&& item){
		insert_or_assign(std::move(item.first), std::move(item.second));
	});
	return *this;
}

Maike::KeyValueStore::JsonHandle Maike::toJson(EnvStore const& env)
{
	KeyValueStore::Compound ret;
	std::for_each(std::begin(env), std::end(env), [&ret](auto const& item){
		ret.set(item.first.c_str(), item.second.c_str());
	});
	return ret.takeHandle();
}

Maike::Env::StringPointers Maike::EnvStore::makeEnviron() const
{
	return Env::StringPointers{*this};
}

