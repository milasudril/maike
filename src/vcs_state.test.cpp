//@	{"targets":[{"name":"vcs_state.test","type":"application", "autorun": 1}]}

#undef NDEBUG

#include "./vcs_state.hpp"

#include <cassert>

namespace
{
	struct TestVcsSource
	{
		static constexpr char const* revision()
		{
			return "Revision";
		}

		static constexpr char const* versionTag()
		{
			return "Version tag";
		}

		static constexpr char const* branch()
		{
			return "Branch";
		}
	};
}

namespace Testcases
{
	void maikeVcsStateInit()
	{
		Maike::VcsState state{TestVcsSource{}};

		assert(state.revision() == TestVcsSource::revision());
		assert(state.versionTag() == TestVcsSource::versionTag());
		assert(state.branch() == TestVcsSource::branch());
	}
}

int main()
{
	Testcases::maikeVcsStateInit();
	return 0;
}
