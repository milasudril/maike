//@	{
//@	"dependencies_extra":[{"ref":"./directory_scanner.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_SOURCETREELOADER_DIRECTORYSCANNER_HPP
#define MAIKE_SOURCETREELOADER_DIRECTORYSCANNER_HPP

#include "./source_file_loader_delegator.hpp"
#include "./input_filter.hpp"

#include "core/utils/fs.hpp"
#include "core/sched/signaling_counter.hpp"
#include "core/sched/thread_pool.hpp"
#include "core/db/source_file_list.hpp"

#include <map>
#include <functional>
#include <forward_list>
#include <memory>

namespace Maike::SourceTreeLoader
{
	class RecursiveScan
	{
	public:
		constexpr explicit RecursiveScan(bool value = true): m_value{value}
		{
		}

		constexpr operator bool() const
		{
			return m_value;
		}

	private:
		bool m_value;
	};

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
		                          std::reference_wrapper<SourceFileLoaderDelegator const> loaders,
		                          RecursiveScan recursive):
		   r_filter{filter}, r_loaders{loaders}, m_recursive{recursive}, m_counter{0}, r_workers{&workers}
		{
		}

		DirectoryScanner& processPath(fs::path const& src_path, fs::path const& target_dir);

		Db::SourceFileList takeResult()
		{
			m_counter.wait(0);
			if(!m_errlog.empty()) { throw ScanException{std::move(m_errlog)}; }
			return std::move(m_source_files);
		}

	private:
		std::reference_wrapper<InputFilter const> r_filter;
		std::reference_wrapper<SourceFileLoaderDelegator const> r_loaders;
		RecursiveScan m_recursive;

		fs::path m_root;

		Db::SourceFileList m_source_files;

		std::shared_mutex m_source_files_mtx;
		Sched::SignalingCounter<size_t> m_counter;
		Sched::ThreadPool* r_workers;

		void processPath(fs::path&& src_path,
		                 fs::path const& target_dir,
		                 std::unique_lock<Sched::SignalingCounter<size_t>> counter);

		std::mutex m_errlog_mtx;
		std::forward_list<std::unique_ptr<char const[]>> m_errlog;
	};
}

#endif
