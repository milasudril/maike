//@	{
//@	 "targets":[{"name":"config.o","type":"object"}]
//@ }

#include "./config.hpp"

#include <algorithm>
#include <string>

Maike::SourceTreeLoader::Config::Config(KeyValueStore::CompoundRefConst obj):
   m_filter{obj.get<KeyValueStore::ArrayRefConst>("input_filter")},
   m_recursive{obj.get<json_int_t>("recursive") != 0}
{
	auto src_file_info_loaders = obj.get<KeyValueStore::CompoundRefConst>("file_info_loaders");
	std::for_each(
	   std::begin(src_file_info_loaders), std::end(src_file_info_loaders), [this](auto item) {
		   m_src_file_info_loaders.insert_or_assign(std::string{item.first},
		                                            std::string{item.second.template as<char const*>()});
	   });
}
