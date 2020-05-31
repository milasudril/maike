//@	{
//@	  "targets":[{"name":"source_file_index.o","type":"object"}]
//@	 }

#include "./source_file_index.hpp"

Maike::Db::SourceFileIndex::RecordConst Maike::Db::SourceFileIndex::insert(fs::path&& path,
                                                                   SourceFileInfo&& info)
{
	auto const id = SourceFileId{m_by_path.size()};
	auto const i = m_by_path.insert(std::make_pair(std::move(path), std::make_pair(id, std::move(info))));
	auto const r_path = std::cref(i.first->first);
	auto const r_info = std::ref(i.first->second.second);
	return RecordConst{id, r_path, r_info};
}

std::vector<Maike::Db::SourceFileIndex::RecordConst> Maike::Db::getRecordsById(SourceFileIndex const& index)
{
	std::vector<SourceFileIndex::RecordConst> ret;
	ret.reserve(index.size());
	index.visitByPath([&ret](auto const& item) { ret.push_back(item); });

	std::sort(
	   std::begin(ret), std::end(ret), [](auto const& a, auto const& b) { return a.id().value() < b.id().value(); });

	return ret;
}