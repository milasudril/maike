//@	{"targets":[{"name":"config.test","type":"application", "autorun":1}]}

#include "./config.hpp"

namespace Testcases
{
	void maikeVcsInvokerConfigDefaultGit()
	{
		Maike::VcsInvoker::Config cfg;
	}
}

int main()
{
	Testcases::maikeVcsInvokerConfigDefaultGit();
	return 0;
}
