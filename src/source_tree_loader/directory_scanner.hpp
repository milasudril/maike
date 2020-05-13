//@	{
//@	 "targets":[{"name":"directory_scanner.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"directory_scanner.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_DIRECTORYSCANNER_HPP
#define MAIKE_DIRECTORYSCANNER_HPP

#include "./source_file_loader_delegator.hpp"

#include "src/fs.hpp"
#include "src/input_filter.hpp"
#include "src/signaling_counter.hpp"
#include "src/thread_pool.hpp"

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

		explicit DirectoryScanner(ThreadPool& workers,
		                          std::reference_wrapper<InputFilter const> filter,
		                          std::reference_wrapper<SourceFileLoaderDelegator const> loaders):
		   r_filter{filter},
		   r_loaders{loaders},
		   m_counter{0},
		   r_workers{&workers}
		{
		}

		DirectoryScanner& processPath(fs::path const& src_path);

		std::map<fs::path, SourceFileInfo> takeResult()
		{
			m_counter.wait(0);
			if(!m_errlog.empty()) { throw ScanException{std::move(m_errlog)}; }
			return std::move(m_source_files);
		}

	private:
		std::reference_wrapper<InputFilter const> r_filter;
		std::reference_wrapper<SourceFileLoaderDelegator const> r_loaders;

		std::map<fs::path, SourceFileInfo> m_source_files;

		// TODO: Move these into a policy base class (to make it more efficient in single-threaded
		//       single-threaded mode)
		std::mutex m_source_files_mtx;
		SignalingCounter<size_t> m_counter;
		ThreadPool* r_workers;

		void processPath(fs::path&& src_path, std::unique_lock<SignalingCounter<size_t>> counter);

		std::mutex m_errlog_mtx;
		std::forward_list<std::unique_ptr<char const[]>> m_errlog;
	};
}

#endif