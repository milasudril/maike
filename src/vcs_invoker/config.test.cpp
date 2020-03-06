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
		assert((cfg.getVersionTag() == Maike::Command{"git", {"describe"}}));
		assert((cfg.getBranch() == Maike::Command{"git", {"rev-parse", "--abbrev-ref", "HEAD"}}));
	}
}

int main()
{
	Testcases::maikeVcsInvokerConfigDefaultGit();
	return 0;
}
