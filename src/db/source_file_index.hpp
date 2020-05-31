//@	{
//@	  "targets":[{"name":"source_file_index.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"source_file_index.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_SOURCEFILEINDEX_HPP
#define MAIKE_DB_SOURCEFILEINDEX_HPP

#include "./source_file_record.hpp"
#include "./source_file_info.hpp"
#include "./source_file_id.hpp"

#include "src/fs.hpp"

#include <map>
#include <algorithm>

namespace Maike::Db
{
	class SourceFileIndex
	{
	public:
		using RecordConst = SourceFileRecordConst;

		RecordConst get(fs::path const& path) const
		{
			auto i = m_by_path.find(path);
			if(i == std::end(m_by_path)) { return RecordConst{}; }
			return RecordConst{i->second.first, std::cref(i->first), std::cref(i->second.second)};
		}

		RecordConst insert(fs::path&& path, SourceFileInfo&& info);

		template<class Func>
		void visitByPath(Func&& f) const
		{
			std::for_each(
			   std::begin(m_by_path), std::end(m_by_path), [cb = std::forward<Func>(f)](auto const& item) {
				   cb(RecordConst{item.second.first, std::cref(item.first), std::cref(item.second.second)});
			   });
		}

		size_t size() const
		{
			return m_by_path.size();
		}

	private:
		std::map<fs::path, std::pair<SourceFileId const, SourceFileInfo>> m_by_path;
	};

	std::vector<SourceFileIndex::RecordConst> getRecordsById(SourceFileIndex const&&) = delete;
	std::vector<SourceFileIndex::RecordConst> getRecordsById(SourceFileIndex const&);
}

#endif