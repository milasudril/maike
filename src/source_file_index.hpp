//@	{
//@	  "targets":[{"name":"source_file_index.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"source_file_index.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_SOURCEFILEINDEX_HPP
#define MAIKE_SOURCEFILEINDEX_HPP

#include "./source_file_info.hpp"
#include "./fs.hpp"

#include <map>
#include <algorithm>

namespace Maike
{
	class SourceFileIndex
	{
	public:
		class RecordConst
		{
		public:
			RecordConst(): r_path{nullptr}
			{
			}

			explicit RecordConst(size_t id,
			                     std::reference_wrapper<fs::path const> path,
			                     std::reference_wrapper<SourceFileInfo const> src_file_info):
			   m_id{id},
			   r_path{&path.get()},
			   r_src_file_info{&src_file_info.get()}
			{
			}

			bool valid() const
			{
				return r_path != nullptr;
			}

			auto id() const
			{
				return m_id;
			}

			auto& path() const
			{
				return *r_path;
			}

			auto& sourceFileInfo() const
			{
				return *r_src_file_info;
			}

		private:
			size_t m_id;
			fs::path const* r_path;
			SourceFileInfo const* r_src_file_info;
		};

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
		std::map<fs::path, std::pair<size_t const, SourceFileInfo>> m_by_path;
	};

	std::vector<SourceFileIndex::RecordConst> getRecordsById(SourceFileIndex const&);
}

#endif