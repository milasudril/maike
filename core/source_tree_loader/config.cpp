//@	{
//@	 "targets":[{"name":"config.o","type":"object"}]
//@ }

#include "./config.hpp"

#include <algorithm>
#include <string>

Maike::SourceTreeLoader::Config::Config(KeyValueStore::CompoundRefConst obj):
   m_filter{obj.get<KeyValueStore::ArrayRefConst>("input_filter")},
   m_dir_filter{obj.get<KeyValueStore::ArrayRefConst>("fullpath_input_filter")},
   m_recursive{obj.get<json_int_t>("recursive") != 0}
{
	auto file_info_loaders = obj.get<KeyValueStore::CompoundRefConst>("file_info_loaders");
	std::for_each(std::begin(file_info_loaders), std::end(file_info_loaders), [this](auto item) {
		m_file_info_loaders.insert_or_assign(std::string{item.first},
		                                     std::string{item.second.template as<char const*>()});
	});
}

Maike::KeyValueStore::JsonHandle Maike::SourceTreeLoader::toJson(Config const& cfg)
{
	Maike::KeyValueStore::Compound ret;

	{
		Maike::KeyValueStore::Compound tmp;
		auto const& file_info_loaders = cfg.fileInfoLoaders();
		std::for_each(std::begin(file_info_loaders),
		              std::end(file_info_loaders),
		              [&tmp](auto const& item) { tmp.set(item.first.c_str(), item.second.c_str()); });
		ret.set("file_info_loaders", std::move(tmp));
	}

	ret.set("recursive", cfg.recursive() ? 1 : 0)
	   .set("input_filter", cfg.inputFilter())
	   .set("fullpath_input_filter", cfg.dirFilter());
	return ret.takeHandle();
}
