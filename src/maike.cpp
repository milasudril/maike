//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./build_info.hpp"
#include "./stringutils.hpp"
#include "./local_system_invoker.hpp"
#include "./input_file.hpp"
#include "./fifo.hpp"
#include "./tag_filter.hpp"
#include "./reader.hpp"
#include "./writer.hpp"
#include "./source_file_loader.hpp"
#include "./config.hpp"
#include "key_value_store/compound.hpp"
#include "key_value_store/array.hpp"

#include "src/cxx/source_file_loader.hpp"

#include "./thread_pool.hpp"
#include "./signaling_counter.hpp"

#include <forward_list>
#include <chrono>

Maike::ThreadPool s_threads;

class MkDir
{
public:
	int run(Maike::fs::path const&,
	        std::vector<Maike::fs::path const*> const&,
	        std::vector<Maike::fs::path const*> const&,
	        Maike::CompilationLog&) const
	{
		return 0;
	}

	Maike::KeyValueStore::Compound settings() const
	{
		return Maike::KeyValueStore::Compound{};
	}

	void settings(Maike::KeyValueStore::Compound&&)
	{
	}
};

std::vector<Maike::Dependency> fixNames(Maike::fs::path const& prefix,
                                        std::vector<Maike::Dependency> const& deps)
{
	std::vector<Maike::Dependency> ret;
	ret.reserve(deps.size());
	std::transform(
	   std::begin(deps), std::end(deps), std::back_inserter(ret), [&prefix](auto const& item) {
		   if(item.resolver() == Maike::Dependency::Resolver::InternalLookup)
		   {
			   auto const& str = item.name().string();
			   if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
			   { return Maike::Dependency{(prefix / item.name()).lexically_normal(), item.resolver()}; }
		   }
		   return item;
	   });
	return ret;
}

std::vector<Maike::fs::path> getTargets(Maike::fs::path const& src_dir,
                                        Maike::KeyValueStore::Compound const& tags)
{
	std::vector<Maike::fs::path> ret;
	if(auto target = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("target"); target)
	{
		auto const name = target->template get<char const*>("name");
		ret.push_back(name);
	}

	if(auto targets = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("targets"); targets)
	{
		std::transform(
		   std::begin(*targets), std::end(*targets), std::back_inserter(ret), [&src_dir](auto item) {
			   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
			   return src_dir / Maike::fs::path{val.template get<char const*>("name")};
		   });
	}
	return ret;
}

Maike::SourceFileInfo loadSourceFile(std::vector<Maike::Dependency>&& builtin_deps,
                                     Maike::fs::path const& path,
                                     Maike::SourceFileLoader const& loader)
{
	Maike::Fifo<std::byte> src_fifo;
	Maike::Fifo<std::byte> tags_fifo;

	Maike::InputFile input{path};
	loader.filterInput(
	   Maike::Reader{input}, Maike::SourceOutStream{src_fifo}, Maike::TagsOutStream{tags_fifo});
	tags_fifo.stop();
	src_fifo.stop();

	{

		auto deps = fixNames(path.parent_path(), loader.getDependencies(Maike::Reader{src_fifo}) /*deps_task.take()*/);
		builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
	}

	auto tags = Maike::KeyValueStore::Compound{Maike::Reader{tags_fifo}, path.string()} /*tags_task.take()*/;
	auto targets = getTargets(path.parent_path(), tags);

	auto compiler = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("compiler");
	return Maike::SourceFileInfo{std::move(builtin_deps),
	                             std::move(targets),
	                             compiler ? loader.getCompiler(*compiler) : loader.getCompiler()};
}


std::optional<Maike::SourceFileInfo> loadSourceFile(Maike::fs::path const& path)
{
	std::vector<Maike::Dependency> deps;
	if(!path.parent_path().empty())
	{
		deps.push_back(Maike::Dependency{path.parent_path().lexically_normal(),
		                                 Maike::Dependency::Resolver::InternalLookup});
	}

	if(is_directory(path))
	{
		std::vector<Maike::fs::path> targets;
		targets.push_back(path.lexically_normal());
		return Maike::SourceFileInfo{std::move(deps), std::move(targets), Maike::Compiler{MkDir{}}};
	}

	auto extension = path.extension();
	if(extension == ".cpp" || extension == ".hpp")
	{
		return loadSourceFile(
		   std::move(deps), path, Maike::SourceFileLoader{Maike::Cxx::SourceFileLoader{}});
	}

	return std::optional<Maike::SourceFileInfo>{};
}

template<class Mutex, class Func, class... Args>
auto invokeWithMutex(Mutex& mtx, Func&& f, Args&&... args)
{
	std::lock_guard lock{mtx};
	return std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
}

namespace Maike
{
	class DirectoryScanner
	{
		public:
			void processPath(fs::path&& src_path, InputFilter const& filter);


		private:
			std::map<fs::path, SourceFileInfo> m_source_files;
			std::mutex m_source_files_mtx;
			SignalingCounter<size_t> m_counter;
	};
}

void processPath(Maike::fs::path&& src_path,
                 Maike::InputFilter const& filter,
                 std::map<Maike::fs::path, Maike::SourceFileInfo>& result,
                 std::mutex& result_mtx,
                 Maike::SignalingCounter<size_t>& counter)
{
	if(src_path != "." && filter.match(src_path.filename().c_str()))
	{
		--counter;
		return;
	}

	auto src_path_normal = src_path.lexically_normal();
	auto i = invokeWithMutex(
	   result_mtx, [&result](auto const& item) { return result.find(item); }, src_path_normal);
	if(i != std::end(result))
	{
		--counter;
		return;
	}

	if(auto src_file_info = loadSourceFile(src_path); src_file_info.has_value())
	{
		auto ins = invokeWithMutex(
		   result_mtx,
		   [&result](auto&& src_path, auto&& src_file_info) {
			   return result.insert(std::make_pair(std::move(src_path), std::move(src_file_info)));
		   },
		   std::move(src_path),
		   std::move(*src_file_info));

		if(ins.second && is_directory(ins.first->first))
		{
			auto i = Maike::fs::directory_iterator{ins.first->first};
			std::for_each(begin(i), end(i), [&filter, &result, &result_mtx, &counter](auto&& item) {
				++counter;
				s_threads.addTask([src_path = item.path(), &filter, &result, &result_mtx, &counter]() mutable {
					processPath(std::move(src_path), filter, result, result_mtx, counter);
				});
			});
		}
	}
	--counter;
}


std::map<Maike::fs::path, Maike::SourceFileInfo>
loadSourceFiles(Maike::InputFilter const& filter,
                Maike::fs::path const& start_dir = Maike::fs::path{"."})
{
	std::map<Maike::fs::path, Maike::SourceFileInfo> ret;
	std::mutex ret_mutex;
	Maike::SignalingCounter<size_t> tasks_running{0};
	auto now = std::chrono::steady_clock::now();
	++tasks_running;
	s_threads.addTask([src_path = start_dir, &filter, &ret, &ret_mutex, &tasks_running]() mutable {
		processPath(std::move(src_path), filter, ret, ret_mutex, tasks_running);
	});
	tasks_running.wait(0);
	fprintf(stderr,
	        "Initial scan took %.7f s\n",
	        std::chrono::duration<double>(std::chrono::steady_clock::now() - now).count());
	return ret;
}

void makeSourceFileInfosFromTargets(std::map<Maike::fs::path, Maike::SourceFileInfo>& source_files)
{
	std::for_each(std::begin(source_files), std::end(source_files), [&source_files](auto const& item) {
		auto const& targets = item.second.targets();
		std::for_each(std::begin(targets), std::end(targets), [&source_files, &item](auto const& target) {
			if(item.first != target) // For backwards compatiblity with old maike
			{
				auto i = source_files.find(target);
				if(i != std::end(source_files)) { throw std::runtime_error{"Target has already been defined"}; }

				std::vector<Maike::Dependency> deps{
				   Maike::Dependency{item.first, Maike::Dependency::Resolver::InternalLookup}};
				Maike::SourceFileInfo src_file;
				src_file.usedFiles(std::move(deps));
				// TODO: Should target dir be prepended to target?
				source_files.insert(i, std::make_pair(target, std::move(src_file)));
			}
		});
	});
}

void resolveDependencies(std::map<Maike::fs::path, Maike::SourceFileInfo>& source_files)
{
	std::for_each(std::begin(source_files), std::end(source_files), [&source_files](auto& item) {
		auto const& deps = item.second.usedFiles();
		std::vector<Maike::Dependency> deps_resolved;
		deps_resolved.reserve(deps.size());
		std::transform(std::begin(deps),
		               std::end(deps),
		               std::back_inserter(deps_resolved),
		               [&source_files](auto const& edge) {
			               auto tmp = edge;
			               return tmp.resolve(source_files);
		               });
		item.second.usedFiles(std::move(deps_resolved));
	});
}


int main()
{
	Maike::KeyValueStore::init();
	Maike::Config cfg;


	//	Maike::LocalSystemInvoker invoker;

	// Current state

	/*	Maike::BuildInfo bi{Maike::VcsState{getStateVariables(std::cref(vcs), invoker)}};

	 store(toJson(bi), BuildInfoSink{});

	 printf(
	    "\n\n#       Start time: %s\n"
	    "#               Id: %s\n"
	    "#     VCS revision: %s\n"
	    "#  VCS version tag: %s\n"
	    "#       VCS branch: %s\n",
	    Maike::toString(bi.startTime()).c_str(),
	    toString(bi.buildId()).c_str(),
	    bi.vcsState().revision().c_str(),
	    bi.vcsState().versionTag().c_str(),
	    bi.vcsState().branch().c_str());
	 fflush(stdout);*/

	auto src_files = loadSourceFiles(cfg.inputFilter());
	printf("%zu\n", src_files.size());
	makeSourceFileInfosFromTargets(src_files);
	resolveDependencies(src_files);
#if 0
	std::for_each(std::begin(src_files), std::end(src_files), [](auto const& item) {
		auto const& deps = item.second.usedFiles();
		printf("\"%s\"\n", item.first.c_str());
		std::for_each(std::begin(deps), std::end(deps), [&item](auto const& edge) {
			if(edge.sourceFile() != nullptr)
			{ printf("\"%s\" -> \"%s\"\n", item.first.c_str(), edge.name().c_str()); }
		});
	});
#endif
}
