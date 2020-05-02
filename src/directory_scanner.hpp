//@	{
//@	 "targets":[{"name":"directory_scanner.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"directory_scanner.o","rel":"implementation"}]
//@ }

#ifndef MAIKE_DIRECTORYSCANNER_HPP
#define MAIKE_DIRECTORYSCANNER_HPP

#include "./fs.hpp"
#include "./input_filter.hpp"
#include "./source_file_loader_delegator.hpp"
#include "./signaling_counter.hpp"
#include "./thread_pool.hpp"

#include <map>
#include <functional>

namespace Maike
{
	class DirectoryScanner
	{
	public:
		static constexpr unsigned int Recursive = 0x1;
		static constexpr unsigned int FollowSymlinks = 0x2;
		static constexpr unsigned int AcceptSockets = 0x4;
		static constexpr unsigned int AcceptFifos = 0x8;

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
			return std::move(m_source_files);
		}

	private:
		std::reference_wrapper<InputFilter const> r_filter;
		std::reference_wrapper<SourceFileLoaderDelegator const> r_loaders;

		std::map<fs::path, SourceFileInfo> m_source_files;
		std::mutex m_source_files_mtx;
		SignalingCounter<size_t> m_counter;
		ThreadPool* r_workers;

		void processPath(fs::path&& src_path, std::unique_lock<SignalingCounter<size_t>> counter);
	};
}

#endif