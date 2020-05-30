//@	{
//@	 "targets":[{"name":"directory_scanner.o","type":"object"}]
//@ }

#include "./directory_scanner.hpp"

#include "src/utils/with_mutex.hpp"

Maike::SourceTreeLoader::DirectoryScanner&
Maike::SourceTreeLoader::DirectoryScanner::processPath(fs::path const& src_path)
{
	r_workers->addTask(
	   [this,
	    src_path = fs::path{src_path},
	    counter = std::unique_lock<Sched::SignalingCounter<size_t>>(m_counter)]() mutable {
		   this->processPath(std::move(src_path), std::move(counter));
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
   fs::path&& src_path, std::unique_lock<Sched::SignalingCounter<size_t>> counter)
{
	try
	{
		if(src_path != "." && r_filter.get().match(src_path.filename().c_str())) { return; }

		auto src_path_normal = src_path.lexically_normal();
		auto res = invokeWithMutex<std::lock_guard>(
		   m_source_files_mtx,
		   [this](auto const& item) { return m_source_files.get(item); },
		   src_path_normal);
		if(res.valid()) { return; }

		if(auto src_file_info = r_loaders.get().load(src_path); src_file_info.has_value())
		{
			auto rec = invokeWithMutex<std::lock_guard>(
			   m_source_files_mtx,
			   [this](auto&& src_path, auto&& src_file_info) {
				   return m_source_files.insert(std::move(src_path), std::move(src_file_info));
			   },
			   std::move(src_path_normal),
			   std::move(*src_file_info));

			if(is_directory(rec.path()))
			{
				auto i = Maike::fs::directory_iterator{rec.path()};
				std::for_each(begin(i), end(i), [this, &counter](auto&& item) {
					r_workers->addTask(
					   [this,
					    src_path = item.path(),
					    counter = std::unique_lock<Sched::SignalingCounter<size_t>>(*counter.mutex())]() mutable {
						   this->processPath(std::move(src_path), std::move(counter));
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