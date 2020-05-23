//@	{
//@	 "targets":[{"name":"pkgconfig_cache.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./pkgconfig_cache.hpp"

#include "src/exec/local_system_invoker.hpp"

#include <cassert>

namespace
{
	struct DummyInvoker
	{
		std::string_view stdout;
		std::string_view stderr;
		Maike::Exec::Result result;

		Maike::fs::path expected_exe;
		std::vector<std::string> expected_args;
	};

	template<class IoRedirector>
	Maike::Exec::Result execp(DummyInvoker const& invoker,
	                          Maike::fs::path exe,
	                          std::vector<std::string> const& args,
	                          IoRedirector& redir)
	{
		assert(exe == invoker.expected_exe);
		assert(args == invoker.expected_args);
		redir(invoker.stdout.begin(), invoker.stdout.size(), Maike::Io::Redirector::StdOut{});
		redir(invoker.stdout.begin(), invoker.stdout.size(), Maike::Io::Redirector::StdErr{});
		return invoker.result;
	}
}

namespace Testcases
{
	void maikePkgconfigCacheCreateWithLocalSystemInvoker()
	{
		Maike::Exec::LocalSystemInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};
	}

	void maikePkgconfigCacheGetIncdirs()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--cflags-only-I", "foobar"};
		invoker.stdout = "-Ifoo\\ bar -Ibaz";
		auto res_a = cache.incdirs("foobar");
		assert((res_a == std::vector<Maike::fs::path>{"foo bar", "baz"}));

		invoker.expected_exe.clear();
		auto res_b = cache.incdirs("foobar");
		assert(res_a == res_b);
	}

	void maikePkgconfigCacheGetIncdirsBadOutput()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--cflags-only-I", "foobar"};
		invoker.stdout = "-Ifoo\\ bar Ibaz";
		try
		{
			(void)cache.incdirs("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetIncdirsOutputShort()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--cflags-only-I", "foobar"};
		invoker.stdout = "-Ifoo\\ bar -I";
		try
		{
			(void)cache.incdirs("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetLibdirs()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--libs-only-L", "foobar"};
		invoker.stdout = "-Lfoo\\ bar -Lbaz";
		auto res_a = cache.libdirs("foobar");
		assert((res_a == std::vector<Maike::fs::path>{"foo bar", "baz"}));

		invoker.expected_exe.clear();
		auto res_b = cache.libdirs("foobar");
		assert(res_a == res_b);
	}

	void maikePkgconfigCacheGetLibdirsBadOutput()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--libs-only-L", "foobar"};
		invoker.stdout = "-Ifoo\\ bar Ibaz";
		try
		{
			(void)cache.libdirs("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetLibdirsOutputShort()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--libs-only-L", "foobar"};
		invoker.stdout = "-Lfoo\\ bar -L";
		try
		{
			(void)cache.libdirs("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetLibs()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--libs", "foobar"};
		invoker.stdout = "-lfoo\\ bar -lbaz";
		auto res_a = cache.libs("foobar");
		assert((res_a
		        == std::vector<Maike::Dependency>{
		           Maike::Dependency{"foo bar", Maike::Dependency::Resolver::None},
		           Maike::Dependency{"baz", Maike::Dependency::Resolver::None}}));

		invoker.expected_exe.clear();
		auto res_b = cache.libs("foobar");
		assert(res_a == res_b);
	}

	void maikePkgconfigCacheGetLibsBadOutput()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--libs", "foobar"};
		invoker.stdout = "-Lfoo\\ bar Lbaz";
		try
		{
			(void)cache.libs("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetLibsOutputShort()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--libs", "foobar"};
		invoker.stdout = "-lfoo\\ bar -l";
		try
		{
			(void)cache.libs("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetCfags()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--cflags-only-other", "foobar"};
		invoker.stdout = "-foo\\ bar -baz";
		auto res_a = cache.cflags("foobar");
		assert((res_a == std::vector<std::string>{"foo bar", "baz"}));

		invoker.expected_exe.clear();
		auto res_b = cache.cflags("foobar");
		assert(res_a == res_b);
	}

	void maikePkgconfigCacheGetCfagsBadOutput()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--cflags-only-other", "foobar"};
		invoker.stdout = "-foo\\ bar baz";
		try
		{
			(void)cache.cflags("foobar");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikePkgconfigCacheGetCfagsOutputShort()
	{
		DummyInvoker invoker;
		Maike::PkgConfig::Cache cache{std::cref(invoker)};

		invoker.result.exitStatus(0);
		invoker.expected_exe = "pkg-config";
		invoker.expected_args = std::vector<std::string>{"--cflags-only-other", "foobar"};
		invoker.stdout = "-foo\\ bar -";
		try
		{
			(void)cache.cflags("foobar");
			abort();
		}
		catch(...)
		{
		}
	}
}

int main()
{
	Testcases::maikePkgconfigCacheCreateWithLocalSystemInvoker();

	Testcases::maikePkgconfigCacheGetIncdirs();
	Testcases::maikePkgconfigCacheGetIncdirsBadOutput();
	Testcases::maikePkgconfigCacheGetIncdirsOutputShort();

	Testcases::maikePkgconfigCacheGetLibdirs();
	Testcases::maikePkgconfigCacheGetLibdirsBadOutput();
	Testcases::maikePkgconfigCacheGetLibdirsOutputShort();

	Testcases::maikePkgconfigCacheGetLibs();
	Testcases::maikePkgconfigCacheGetLibsBadOutput();
	Testcases::maikePkgconfigCacheGetLibsOutputShort();

	Testcases::maikePkgconfigCacheGetCfags();
	Testcases::maikePkgconfigCacheGetCfagsBadOutput();
	Testcases::maikePkgconfigCacheGetCfagsOutputShort();
	return 0;
}