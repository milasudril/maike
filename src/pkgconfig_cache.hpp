//@	{
//@	 "targets":[{"name":"pkgconfig_cache.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"pkgconfig_cache.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PKGCONFIGCACHE_HPP
#define MAIKE_PKGCONFIGCACHE_HPP

#include "./fs.hpp"
#include "./source_file_info.hpp"
#include "src/io/mem_io_redirector.hpp"
#include "src/exec/result.hpp"

#include "src/utils/with_mutex.hpp"

#include <map>
#include <shared_mutex>
#include <string>
#include <type_traits>

namespace Maike
{
	class PkgConfigCache
	{
	private:
		using IoRedirectorType = Io::MemRedirector;

	public:
		template<class Invoker,
		         std::enable_if_t<!std::is_same_v<std::decay_t<PkgConfigCache>, Invoker>, int> = 0>
		explicit PkgConfigCache(std::reference_wrapper<Invoker> invoker):
		   r_invoker{&invoker.get()},
		   r_execp{[](void const* invoker,
		              fs::path const& exe,
		              std::vector<std::string> const& args,
		              IoRedirectorType& redir) {
			   auto self = reinterpret_cast<Invoker*>(invoker);
			   return execp(*self, exe, args, redir);
		   }}
		{
		}

		std::vector<fs::path> const& incdirs(std::string_view libname) const;
		std::vector<std::string> const& cflags(std::string_view libname) const;
		std::vector<fs::path> const& libdirs(std::string_view libname) const;
		std::vector<Dependency> const& libs(std::string_view libname) const;

	private:
		mutable std::shared_mutex m_incdirs_mtx;
		mutable std::map<std::string, std::vector<fs::path>, std::less<>> m_incdirs;
		mutable std::shared_mutex m_cflags_mtx;
		mutable std::map<std::string, std::vector<std::string>, std::less<>> m_cflags;
		mutable std::shared_mutex m_libdirs_mtx;
		mutable std::map<std::string, std::vector<fs::path>, std::less<>> m_libdirs;
		mutable std::shared_mutex m_libs_mtx;
		mutable std::map<std::string, std::vector<Dependency>, std::less<>> m_libs;

		void const* r_invoker;
		Exec::Result (*r_execp)(void const* invoker,
		                        fs::path const& exe,
		                        std::vector<std::string> const& args,
		                        IoRedirectorType& redir);
	};
}

#endif