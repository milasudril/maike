//@	{"targets":[{"name":"config.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./config.hpp"
#include <cassert>

namespace Testcases
{
	void maikeVcsInvokerConfigDefaultGit()
	{
		Maike::VcsInvoker::Config cfg;

		assert((cfg.getRevision() == Maike::Command{"git", {"rev-parse", "HEAD"}}));
		assert((cfg.getVersionTag() == Maike::Command{"git", {"describe", "--dirty"}}));
		assert((cfg.getBranch() == Maike::Command{"git", {"rev-parse", "--abbrev-ref", "HEAD"}}));
	}

	void maikeVcsInvokerSetGetRevision()
	{
		Maike::VcsInvoker::Config cfg;
		auto get_version_tag = cfg.getVersionTag();
		auto get_branch = cfg.getBranch();

		cfg.getRevision(Maike::Command{"foo", {"bar", "kaka"}});

		assert((cfg.getRevision() == Maike::Command{"foo", {"bar", "kaka"}}));
		assert(cfg.getVersionTag() == get_version_tag);
		assert(cfg.getBranch() == get_branch);
	}

	void maikeVcsInvokerSetGetVersionTag()
	{
		Maike::VcsInvoker::Config cfg;
		auto get_revision = cfg.getRevision();
		auto get_branch = cfg.getBranch();
		cfg.getVersionTag(Maike::Command{"foo", {"bar", "kaka"}});

		assert((cfg.getVersionTag() == Maike::Command{"foo", {"bar", "kaka"}}));
		assert(cfg.getRevision() == get_revision);
		assert(cfg.getBranch() == get_branch);
	}


	void maikeVcsInvokerSetGetBranch()
	{
		Maike::VcsInvoker::Config cfg;
		auto get_revision = cfg.getRevision();
		auto get_version_tag = cfg.getVersionTag();

		cfg.getBranch(Maike::Command{"foo", {"bar", "kaka"}});
		assert((cfg.getBranch() == Maike::Command{"foo", {"bar", "kaka"}}));
		assert(cfg.getRevision() == get_revision);
		assert(cfg.getVersionTag() == get_version_tag);
	}
}

int main()
{
	Testcases::maikeVcsInvokerConfigDefaultGit();
	Testcases::maikeVcsInvokerSetGetRevision();
	Testcases::maikeVcsInvokerSetGetVersionTag();
	Testcases::maikeVcsInvokerSetGetBranch();
	return 0;
}
