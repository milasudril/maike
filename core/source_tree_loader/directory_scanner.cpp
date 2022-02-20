//@	{
//@	 "targets":[{"name":"directory_scanner.o","type":"object"}]
//@ }

#include "./directory_scanner.hpp"

#include "core/utils/with_mutex.hpp"

Maike::SourceTreeLoader::DirectoryScanner&
Maike::SourceTreeLoader::DirectoryScanner::processPath(fs::path const& src_path,
                                                       fs::path const& target_dir)
{
	m_root = src_path;

	r_workers->addTask(
	   [this,
	    src_path = fs::path{src_path},
	    target_dir,
	    // NOTE: We use std::unique_lock to make sure that the counter is decreased when the task
	    // finishes.
	    counter = std::unique_lock<Sched::SignalingCounter<size_t>>(m_counter)]() mutable {
		   this->processPath(std::move(src_path), target_dir, std::move(counter));
	   });

	return *this;
}

Maike::SourceTreeLoader::DirectoryScanner::ScanException::ScanException(
   std::forward_list<std::unique_ptr<char const[]>>&& errlog)
{
	std::for_each(std::begin(errlog), std::end(errlog), [this](auto&& item) {
		m_errors += item.get();
		m_errors += '\n';
	});
}

void Maike::SourceTreeLoader::DirectoryScanner::processPath(
   fs::path&& src_path,
   fs::path const& target_dir,
   std::unique_lock<Sched::SignalingCounter<size_t>> counter)
{
	try
	{
		if(src_path != m_root)
		{
			if(!m_recursive && is_directory(src_path)) { return; }

			if(r_filter.get().match(src_path.filename().c_str())) { return; }

			if(r_dir_filter.get().match((src_path.lexically_relative(m_root)).c_str())) { return; }
		}

		auto src_path_normal = src_path.lexically_normal();
		auto i = invokeWithMutex<std::shared_lock>(
		   m_source_files_mtx,
		   [this](auto const& item) { return m_source_files.find(item); },
		   src_path_normal);
		if(i != std::end(m_source_files)) { return; }

		auto src_file_info = r_loaders.get().load(src_path, m_root, target_dir);

		auto ins = invokeWithMutex<std::lock_guard>(
		   m_source_files_mtx,
		   [&src_files = m_source_files](auto&& src_path, auto&& src_file_info) {
			   return src_files.insert(std::make_pair(std::move(src_path), std::move(src_file_info)));
		   },
		   std::move(src_path_normal),
		   std::move(src_file_info));

		if(ins.second && is_directory(ins.first->first))
		{
			auto i = Maike::fs::directory_iterator{ins.first->first};
			std::for_each(begin(i), end(i), [this, &target_dir, &counter](auto&& item) {
				r_workers->addTask(
				   [this,
				    src_path = item.path(),
				    target_dir,
				    counter = std::unique_lock<Sched::SignalingCounter<size_t>>(*counter.mutex())]() mutable {
					   this->processPath(std::move(src_path), target_dir, std::move(counter));
				   });
			});
		}
	}
	catch(std::exception const& err)
	{
		char const* msg = err.what();
		auto l = strlen(msg) + 1;
		auto ptr = std::make_unique<char[]>(l);
		memcpy(ptr.get(), msg, l);

		std::lock_guard lock{m_errlog_mtx};
		m_errlog.push_front(std::move(ptr));
	}
}