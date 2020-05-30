//@	{
//@	  "targets":[{"name":"source_file_index.o","type":"object"}]
//@	 }

#include "./source_file_index.hpp"

Maike::SourceFileIndex::RecordConst Maike::SourceFileIndex::insert(fs::path&& path,
                                                                   SourceFileInfo&& info)
{
	auto id = m_by_path.size();
	auto i = m_by_path.insert(std::make_pair(std::move(path), std::make_pair(id, std::move(info))));
	auto r_path = std::cref(i.first->first);
	auto r_info = std::ref(i.first->second.second);
	if(i.second) { m_by_id.insert(std::make_pair(id, std::make_pair(r_path, r_info))); }
	return RecordConst{id, r_path, r_info};
}
