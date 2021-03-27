//@	{
//@	  "targets":[{"name":"source_file_record.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_SOURCEFILERECORD_HPP
#define MAIKE_DB_SOURCEFILERECORD_HPP

#include "./source_file_id.hpp"
#include "./source_file_info.hpp"

#include "src/fs.hpp"

#include <functional>

namespace Maike::Db
{
	class SourceFileRecordConst
	{
	public:
		SourceFileRecordConst(): r_path{nullptr}
		{
		}

		explicit SourceFileRecordConst(SourceFileId id,
		                               std::reference_wrapper<fs::path const> path,
		                               std::reference_wrapper<SourceFileInfo const> src_file_info):
		   m_id{id}, r_path{&path.get()}, r_src_file_info{&src_file_info.get()}
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
		SourceFileId m_id;
		fs::path const* r_path;
		SourceFileInfo const* r_src_file_info;
	};

	template<class Function>
	void visitEdges(Function&& f, SourceFileRecordConst const& rec)
	{
		assert(rec.valid());
		auto const& deps = rec.sourceFileInfo().useDeps();
		std::for_each(std::begin(deps), std::end(deps), std::forward<Function>(f));
	}

	inline auto id(SourceFileRecordConst const& rec)
	{
		return rec.id();
	}
}

#endif