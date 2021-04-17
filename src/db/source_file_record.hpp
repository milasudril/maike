//@	{
//@	  "targets":[{"name":"source_file_record.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"source_file_record.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DB_SOURCEFILERECORD_HPP
#define MAIKE_DB_SOURCEFILERECORD_HPP

#include "./source_file_id.hpp"
#include "./source_file_info.hpp"

#include "src/utils/fs.hpp"
#include "src/build/info.hpp"

#include <algorithm>

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

	struct UseDepsOnly
	{
	};

	template<class Function>
	void visitEdges(Function&& f, SourceFileRecordConst const& rec)
	{
		assert(rec.valid());
		auto const& use_deps = rec.sourceFileInfo().useDeps();
		std::for_each(std::begin(use_deps), std::end(use_deps), f);

		auto const& build_deps = rec.sourceFileInfo().buildDeps();
		std::for_each(std::begin(build_deps), std::end(build_deps), f);
	}

	template<class Function>
	void visitEdges(Function&& f, SourceFileRecordConst const& rec, UseDepsOnly)
	{
		assert(rec.valid());
		auto const& use_deps = rec.sourceFileInfo().useDeps();
		std::for_each(std::begin(use_deps), std::end(use_deps), f);
	}

	template<class Function, class EdgeCallback>
	void visitEdges(Function&& f, SourceFileRecordConst const& rec, EdgeCallback&& ec)
	{
		assert(rec.valid());
		auto const& use_deps = rec.sourceFileInfo().useDeps();
		std::for_each(
		   std::begin(use_deps),
		   std::end(use_deps),
		   [func = std::forward<Function>(f), edge = std::forward<EdgeCallback>(ec)](auto const& item) {
			   edge(item);
			   func(item);
		   });
	}

	inline auto id(SourceFileRecordConst const& rec)
	{
		return rec.id();
	}

	bool isUpToDate(SourceFileRecordConst const& rec, std::vector<Dependency> const& deps);

	// FIXME: Pass invoker, and env
	void compile(SourceFileRecordConst const& rec,
	             Build::Info const&,
	             std::vector<Dependency> const& deps);
}

#endif