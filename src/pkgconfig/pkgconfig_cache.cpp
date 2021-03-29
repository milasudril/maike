//@	{
//@	 "targets":[{"name":"pkgconfig_cache.o","type":"object"}]
//@	}

#include "./pkgconfig_cache.hpp"

#include "src/utils/stringutils.hpp"
#include "src/utils/with_mutex.hpp"

#include <stdexcept>

namespace
{
	template<class Callback>
	std::vector<std::string>
	pkgconfig_run(void const* invoker, Callback cb, std::string_view action, std::string_view libname)
	{
		Maike::PkgConfig::IoRedirector redir;
		if(auto exec_result = cb(invoker,
		                         "pkg-config",
		                         std::vector<std::string>{std::string{action}, std::string{libname}},
		                         redir);
		   failed(exec_result))
		{
			//	FIXME: Add ExecException class
			throw std::runtime_error{std::string{"pkg-config request failed: "} + redir.stderr()};
		}

		return redir.takeStdout();
	}

	template<class Callback>
	std::vector<Maike::fs::path>
	fetch_incdirs(void const* invoker, Callback cb, std::string_view libname)
	{
		auto cmd_out = pkgconfig_run(invoker, cb, "--cflags-only-I", libname);
		std::vector<Maike::fs::path> ret;
		ret.reserve(cmd_out.size());
		std::transform(
		   std::begin(cmd_out), std::end(cmd_out), std::back_inserter(ret), [](auto const& val) {
			   if(val.size() < 3 || !(val[0] == '-' && val[1] == 'I'))
			   { throw std::runtime_error{std::string{"Unexpected format in pkg-config output: "} + val}; }
			   // Strip "-I"
			   return val.c_str() + 2;
		   });

		return ret;
	}


	template<class Callback>
	std::vector<Maike::fs::path>
	fetch_libdirs(void const* invoker, Callback cb, std::string_view libname)
	{
		auto cmd_out = pkgconfig_run(invoker, cb, "--libs-only-L", libname);
		std::vector<Maike::fs::path> ret;
		ret.reserve(cmd_out.size());
		std::transform(
		   std::begin(cmd_out), std::end(cmd_out), std::back_inserter(ret), [](auto const& val) {
			   if(val.size() < 3 || !(val[0] == '-' && val[1] == 'L'))
			   { throw std::runtime_error{std::string{"Unexpected format in pkg-config output: "} + val}; }
			   // Strip "-L"
			   return val.c_str() + 2;
		   });

		return ret;
	}

	template<class Callback>
	std::vector<Maike::fs::path> fetch_libs(void const* invoker, Callback cb, std::string_view libname)
	{
		auto cmd_out = pkgconfig_run(invoker, cb, "--libs", libname);
		std::vector<Maike::fs::path> ret;
		ret.reserve(cmd_out.size());
		std::transform(
		   std::begin(cmd_out), std::end(cmd_out), std::back_inserter(ret), [](auto const& val) {
			   if(val.size() < 3 || !(val[0] == '-' && val[1] == 'l'))
			   { throw std::runtime_error{std::string{"Unexpected format in pkg-config output: "} + val}; }
			   // Strip "-l"
			   return Maike::fs::path{val.c_str() + 2};
		   });

		return ret;
	}

	template<class Callback>
	std::vector<std::string> fetch_cflags(void const* invoker, Callback cb, std::string_view libname)
	{
		auto cmd_out = pkgconfig_run(invoker, cb, "--cflags-only-other", libname);
		std::vector<std::string> ret;
		ret.reserve(cmd_out.size());
		std::transform(
		   std::begin(cmd_out), std::end(cmd_out), std::back_inserter(ret), [](auto const& val) {
			   if(val.size() < 2 || val[0] != '-')
			   { throw std::runtime_error{std::string{"Unexpected format in pkg-config output: "} + val}; }
			   // Strip "-"
			   return val.c_str() + 1;
		   });

		return ret;
	}
}

std::vector<Maike::fs::path> const& Maike::PkgConfig::Cache::incdirs(std::string_view libname) const
{
	auto i = invokeWithMutex<std::shared_lock>(
	   m_incdirs_mtx, [this](auto libname) { return m_incdirs.find(libname); }, libname);
	if(i == std::end(m_incdirs))
	{
		auto res = fetch_incdirs(r_invoker, r_execp, libname);
		i = invokeWithMutex<std::lock_guard>(
		   m_incdirs_mtx,
		   [this, incdirs = std::move(res)](auto libname) {
			   return m_incdirs.insert(std::make_pair(libname, std::move(incdirs))).first;
		   },
		   libname);
	}
	return i->second;
}

std::vector<Maike::fs::path> const& Maike::PkgConfig::Cache::libdirs(std::string_view libname) const
{
	auto i = invokeWithMutex<std::shared_lock>(
	   m_libdirs_mtx, [this](auto libname) { return m_libdirs.find(libname); }, libname);
	if(i == std::end(m_libdirs))
	{
		auto res = fetch_libdirs(r_invoker, r_execp, libname);
		i = invokeWithMutex<std::lock_guard>(
		   m_libdirs_mtx,
		   [this, incdirs = std::move(res)](auto libname) {
			   return m_libdirs.insert(std::make_pair(libname, std::move(incdirs))).first;
		   },
		   libname);
	}
	return i->second;
}

std::vector<Maike::fs::path> const& Maike::PkgConfig::Cache::libs(std::string_view libname) const
{
	auto i = invokeWithMutex<std::shared_lock>(
	   m_libs_mtx, [this](auto libname) { return m_libs.find(libname); }, libname);
	if(i == std::end(m_libs))
	{
		auto res = fetch_libs(r_invoker, r_execp, libname);
		i = invokeWithMutex<std::lock_guard>(
		   m_libs_mtx,
		   [this, incdirs = std::move(res)](auto libname) {
			   return m_libs.insert(std::make_pair(libname, std::move(incdirs))).first;
		   },
		   libname);
	}
	return i->second;
}

std::vector<std::string> const& Maike::PkgConfig::Cache::cflags(std::string_view libname) const
{
	auto i = invokeWithMutex<std::shared_lock>(
	   m_cflags_mtx, [this](auto libname) { return m_cflags.find(libname); }, libname);
	if(i == std::end(m_cflags))
	{
		auto res = fetch_cflags(r_invoker, r_execp, libname);
		i = invokeWithMutex<std::lock_guard>(
		   m_cflags_mtx,
		   [this, incdirs = std::move(res)](auto libname) {
			   return m_cflags.insert(std::make_pair(libname, std::move(incdirs))).first;
		   },
		   libname);
	}
	return i->second;
}