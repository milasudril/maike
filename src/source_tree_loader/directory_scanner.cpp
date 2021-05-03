//@	{
//@	 "targets":[{"name":"directory_scanner.o","type":"object"}]
//@ }

#include "./directory_scanner.hpp"

#include "src/utils/with_mutex.hpp"

Maike::SourceTreeLoader::DirectoryScanner&
Maike::SourceTreeLoader::DirectoryScanner::processPath(fs::path const& src_path,
                                                       fs::path const& target_dir)
{
	m_root = src_path;

	r_workers->addTask([this,
	                    src_path = fs::path{src_path},
	                    counter = std::unique_lock<Sched::SignalingCounter<size_t>>(m_counter),
	                    target_dir]() mutable {
		this->processPath(std::move(src_path), std::move(counter), target_dir);
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
   std::unique_lock<Sched::SignalingCounter<size_t>> counter,
   fs::path const& target_dir)
{
	try
	{
		if(src_path != m_root && r_filter.get().match(src_path.filename().c_str())) { return; }

		auto src_path_normal = src_path.lexically_normal();
		auto i = invokeWithMutex<std::shared_lock>(
		   m_source_files_mtx,
		   [this](auto const& item) { return m_source_files.find(item); },
		   src_path_normal);
		if(i != std::end(m_source_files)) { return; }

		if(auto src_file_info = r_loaders.get().load(m_root, src_path, target_dir); src_file_info)
		{
			auto ins = invokeWithMutex<std::lock_guard>(
			   m_source_files_mtx,
			   [&src_files = m_source_files](auto&& src_path, auto&& src_file_info) {
				   return src_files.insert(std::make_pair(std::move(src_path), std::move(src_file_info)));
			   },
			   std::move(src_path_normal),
			   std::move(*src_file_info));

			if(ins.second && is_directory(ins.first->first))
			{
				auto i = Maike::fs::directory_iterator{ins.first->first};
				std::for_each(begin(i), end(i), [this, &counter, &target_dir](auto&& item) {
					r_workers->addTask(
					   [this,
					    src_path = item.path(),
					    target_dir,
					    counter = std::unique_lock<Sched::SignalingCounter<size_t>>(*counter.mutex())]() mutable {
						   this->processPath(std::move(src_path), std::move(counter), target_dir);
					   });
				});
			}
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