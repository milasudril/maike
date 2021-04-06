//@	{
//@	 "targets":[{"name":"directory_scanner.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"directory_scanner.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_SOURCETREELOADER_DIRECTORYSCANNER_HPP
#define MAIKE_SOURCETREELOADER_DIRECTORYSCANNER_HPP

#include "./source_file_loader_delegator.hpp"
#include "./input_filter.hpp"

#include "src/fs.hpp"
#include "src/sched/signaling_counter.hpp"
#include "src/sched/thread_pool.hpp"

#include <map>
#include <functional>
#include <forward_list>
#include <memory>

namespace Maike::SourceTreeLoader
{
	class DirectoryScanner
	{
	public:
		class ScanException: public std::exception
		{
		public:
			explicit ScanException(std::forward_list<std::unique_ptr<char const[]>>&& errlog);

			char const* what() const noexcept override
			{
				return m_errors.data();
			}

		private:
			std::string m_errors;
		};

		static constexpr unsigned int Recursive = 0x1;

		explicit DirectoryScanner(Sched::ThreadPool& workers,
		                          std::reference_wrapper<InputFilter const> filter,
		                          std::reference_wrapper<SourceFileLoaderDelegator const> loaders):
		   r_filter{filter}, r_loaders{loaders}, m_counter{0}, r_workers{&workers}
		{
		}

		DirectoryScanner& processPath(fs::path const& src_path, fs::path const& target_dir);

		std::map<fs::path, Db::SourceFileInfo> takeResult()
		{
			m_counter.wait(0);
			if(!m_errlog.empty()) { throw ScanException{std::move(m_errlog)}; }
			return std::move(m_source_files);
		}

	private:
		std::reference_wrapper<InputFilter const> r_filter;
		std::reference_wrapper<SourceFileLoaderDelegator const> r_loaders;

		std::map<fs::path, Db::SourceFileInfo> m_source_files;

		// TODO: Move these into a policy base class (to make it more efficient in single-threaded
		//       single-threaded mode)
		std::mutex m_source_files_mtx;
		Sched::SignalingCounter<size_t> m_counter;
		Sched::ThreadPool* r_workers;

		void processPath(fs::path&& src_path,
		                 std::unique_lock<Sched::SignalingCounter<size_t>> counter,
		                 fs::path const& target_dir);

		std::mutex m_errlog_mtx;
		std::forward_list<std::unique_ptr<char const[]>> m_errlog;
	};
}

#endif