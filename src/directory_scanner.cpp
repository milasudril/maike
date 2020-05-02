//@	{
//@	 "targets":[{"name":"directory_scanner.o","type":"object"}]
//@ }

#include "./directory_scanner.hpp"

namespace
{
	// TODO: This function should be moved.
	template<class Mutex, class Func, class... Args>
	auto invokeWithMutex(Mutex& mtx, Func&& f, Args&&... args)
	{
		std::lock_guard lock{mtx};
		return std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
	}
}

Maike::DirectoryScanner& Maike::DirectoryScanner::processPath(fs::path const& src_path)
{
	r_workers->addTask(
	   [this,
	    src_path = fs::path{src_path},
	    counter = std::unique_lock<Maike::SignalingCounter<size_t>>(m_counter)]() mutable {
		   this->processPath(std::move(src_path), std::move(counter));
	   });

	return *this;
}

Maike::DirectoryScanner::ScanException::ScanException(
   std::forward_list<std::unique_ptr<char const[]>>&& errlog)
{
	std::for_each(std::begin(errlog), std::end(errlog), [this](auto&& item) {
		m_errors += item.get();
		m_errors += '\n';
	});
}


void Maike::DirectoryScanner::processPath(fs::path&& src_path,
                                          std::unique_lock<SignalingCounter<size_t>> counter)
{
	try
	{
		if(src_path != "." && r_filter.get().match(src_path.filename().c_str())) { return; }

		auto src_path_normal = src_path.lexically_normal();
		auto i = invokeWithMutex(
		   m_source_files_mtx,
		   [this](auto const& item) { return m_source_files.find(item); },
		   src_path_normal);
		if(i != std::end(m_source_files)) { return; }

		if(auto src_file_info = r_loaders.get().load(src_path); src_file_info.has_value())
		{
			auto ins = invokeWithMutex(
			   m_source_files_mtx,
			   [this](auto&& src_path, auto&& src_file_info) {
				   return m_source_files.insert(std::make_pair(std::move(src_path), std::move(src_file_info)));
			   },
			   std::move(src_path),
			   std::move(*src_file_info));

			if(ins.second && is_directory(ins.first->first))
			{
				auto i = Maike::fs::directory_iterator{ins.first->first};
				std::for_each(begin(i), end(i), [this, &counter](auto&& item) {
					r_workers->addTask(
					   [this,
					    src_path = item.path(),
					    counter = std::unique_lock<Maike::SignalingCounter<size_t>>(*counter.mutex())]() mutable {
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