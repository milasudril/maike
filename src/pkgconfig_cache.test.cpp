//@	{
//@	 "targets":[{"name":"pkgconfig_cache.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./pkgconfig_cache.hpp"

#include "src/exec/local_system_invoker.hpp"

#include <cassert>

namespace Testcases
{
	void maikePkgconfigCacheCreateWithLocalSystemInvoker()
	{
		Maike::Exec::LocalSystemInvoker invoker;
		Maike::PkgConfigCache cache{std::cref(invoker)};

		auto res = cache.incdirs("gtk+-3.0");
		std::for_each(
		   std::begin(res), std::end(res), [](auto const& val) { printf("%s\n", val.c_str()); });
	}
}

int main()
{
	Testcases::maikePkgconfigCacheCreateWithLocalSystemInvoker();
	return 0;
}