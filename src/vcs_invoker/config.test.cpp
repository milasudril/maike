//@	{"targets":[{"name":"config.test","type":"application", "autorun":1}]}

#undef NDEBUG

#include "./config.hpp"

#include <cassert>
#include <string>
#include <cstring>

namespace
{
	struct TestSink
	{
		std::string buffer;
	};

	void write(TestSink& s, char const* buffer, size_t n)
	{
		s.buffer.append(buffer, n);
	}

	struct TestSource
	{
		explicit TestSource(char const* buffer):buffer{buffer},read_ptr{buffer}{}
		std::string_view buffer;
		char const* read_ptr;
	};

	size_t read(TestSource& s, std::byte* buffer, size_t N)
	{
		auto n = std::min(N, static_cast<size_t>(s.buffer.end() - s.read_ptr));
		memcpy(buffer,s.read_ptr, n);
		s.read_ptr += n;
		return n;
	}
}

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

	void maikeVcsInvokerConfigToJson()
	{
		Maike::VcsInvoker::Config cfg;
		assert((cfg.getRevision() == Maike::Command{"git", {"rev-parse", "HEAD"}}));
		assert((cfg.getVersionTag() == Maike::Command{"git", {"describe", "--dirty"}}));
		assert((cfg.getBranch() == Maike::Command{"git", {"rev-parse", "--abbrev-ref", "HEAD"}}));

		TestSink s;
		auto json = toJson(cfg);
		store(json, s);

		char const* expected = R"_({
    "get_branch": {
        "args": [
            "rev-parse",
            "--abbrev-ref",
            "HEAD"
        ],
        "executable": "git"
    },
    "get_revision": {
        "args": [
            "rev-parse",
            "HEAD"
        ],
        "executable": "git"
    },
    "get_version_tag": {
        "args": [
            "describe",
            "--dirty"
        ],
        "executable": "git"
    }
})_";

	assert(s.buffer == expected);
	}

	void maikeVcsInvokerConfigFromJson()
	{
		TestSource src{R"_({
    "get_branch": {
        "args": [
            "x",
            "y",
            "z"
        ],
        "executable": "foo"
    },
    "get_revision": {
        "args": [
            "rev-parse",
            "HEAD"
        ],
        "executable": "git"
    },
    "get_version_tag": {
        "args": [
            "describe",
            "--dirty"
        ],
        "executable": "git"
    }
})_"};

		auto json = Maike::KeyValueStore::jsonLoad(src, "");
		assert(json.valid());
		auto cfg = json.as<Maike::VcsInvoker::Config>();
		assert((cfg.getBranch() == Maike::Command{"foo", std::vector<std::string>{"x", "y", "z"}}));

	}
}

int main()
{
	Testcases::maikeVcsInvokerConfigDefaultGit();
	Testcases::maikeVcsInvokerSetGetRevision();
	Testcases::maikeVcsInvokerSetGetVersionTag();
	Testcases::maikeVcsInvokerSetGetBranch();
	Testcases::maikeVcsInvokerConfigToJson();
	Testcases::maikeVcsInvokerConfigFromJson();
	return 0;
}
