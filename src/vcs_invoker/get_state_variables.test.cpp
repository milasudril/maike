//@	{"targets":[{"name":"get_state_variables.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./get_state_variables.hpp"

#include <cassert>

namespace
{
	struct TestInvoker
	{
		Maike::fs::path* exe;
		std::vector<std::string>* args;
	};

	template<class IoRedirector>
	int execp(TestInvoker const& invoker,
	          Maike::fs::path const& exe,
	          std::vector<std::string> const& args,
	          IoRedirector&& redir)
	{
		*invoker.exe = exe;
		*invoker.args = args;
		std::string_view retval{"This is a test"};
		redir(reinterpret_cast<std::byte const*>(retval.data()),
		      retval.size(),
		      Maike::IoRedirector::StdOut{});
		return 0;
	}
}

namespace Testcases
{
	void maikeVcsGetStateVariablesGetRevision()
	{
		Maike::VcsInvoker::Config const cfg;
		Maike::fs::path exe;
		std::vector<std::string> args;
		TestInvoker invoker{&exe, &args};
		auto rev = getRevision(cfg, invoker);

		auto& get_rev_cmd = cfg.getRevision();
		assert(*invoker.exe == get_rev_cmd.executable());
		assert(*invoker.args == get_rev_cmd.args());
		assert(rev == "This is a test");
	}
}

int main()
{
	Testcases::maikeVcsGetStateVariablesGetRevision();
	return 0;
}
