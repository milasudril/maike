//@	{
//@	 "targets":[{"name":"pkgconfig_cache.o","type":"object"}]
//@	}

#include "./pkgconfig_cache.hpp"

#include "src/utils/stringutils.hpp"

namespace
{
	template<class Callback>
	std::vector<Maike::fs::path>
	fetch_incdirs(void const* invoker, Callback cb, std::string_view libname)
	{
		std::vector<Maike::fs::path> ret;
		Maike::Io::MemRedirector redir{nullptr, 0};

		if(auto exec_result =
		      cb(invoker,
		         "pkg-config",
		         std::vector<std::string>{std::string{"--cflags-only-I"}, std::string{libname}},
		         redir);
		   failed(exec_result))
		{
			//	FIXME: Add ExecException class
			throw std::runtime_error{std::string{"pkg-config request failed: "}
			                         + Maike::toString(redir.stderr())};
		}
		printf("%s\n", Maike::toString(redir.stdout()).c_str());

		return ret;
	}
}

std::vector<Maike::fs::path> const& Maike::PkgConfigCache::incdirs(std::string_view libname) const
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