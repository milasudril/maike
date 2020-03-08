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
		std::string_view retval;

		if(exe == "get branch") { retval = "get branch"; }
		else if(exe == "get revision")
		{
			retval = "get revision";
		}
		else if(exe == "get version tag")
		{
			retval = "get version tag";
		}
		else
		{
			retval = "This is a test";
		}
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

	void maikeVcsGetStateVariablesGetVersionTag()
	{
		Maike::VcsInvoker::Config const cfg;
		Maike::fs::path exe;
		std::vector<std::string> args;
		TestInvoker invoker{&exe, &args};
		auto ver = getVersionTag(cfg, invoker);

		auto& get_ver_tag_cmd = cfg.getVersionTag();
		assert(*invoker.exe == get_ver_tag_cmd.executable());
		assert(*invoker.args == get_ver_tag_cmd.args());
		assert(ver == "This is a test");
	}

	void maikeVcsGetStateVariablesGetBrnach()
	{
		Maike::VcsInvoker::Config const cfg;
		Maike::fs::path exe;
		std::vector<std::string> args;
		TestInvoker invoker{&exe, &args};
		auto branch = getBranch(cfg, invoker);

		auto& get_branch_cmd = cfg.getBranch();
		assert(*invoker.exe == get_branch_cmd.executable());
		assert(*invoker.args == get_branch_cmd.args());
		assert(branch == "This is a test");
	}

	void maikeVcsGetStateVariablesGetStateVars()
	{
		Maike::VcsInvoker::Config cfg;
		cfg.getBranch(Maike::Command{"get branch", {}})
		   .getRevision(Maike::Command{"get revision", {}})
		   .getVersionTag(Maike::Command{"get version tag", {}});

		Maike::fs::path exe;
		std::vector<std::string> args;

		TestInvoker invoker{&exe, &args};
		auto res = getStateVariables(cfg, invoker);

		assert(res.branch() == "get branch");
		assert(res.revision() == "get revision");
		assert(res.versionTag() == "get version tag");
	}
}

int main()
{
	Testcases::maikeVcsGetStateVariablesGetRevision();
	Testcases::maikeVcsGetStateVariablesGetVersionTag();
	Testcases::maikeVcsGetStateVariablesGetBrnach();
	Testcases::maikeVcsGetStateVariablesGetStateVars();
	return 0;
}