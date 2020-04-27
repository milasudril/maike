//@	{
//@	  "targets":[{"name":"config.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./config.hpp"

#include <cassert>

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
		explicit TestSource(char const* buffer): buffer{buffer}, read_ptr{buffer}
		{
		}
		std::string_view buffer;
		char const* read_ptr;
	};

	size_t read(TestSource& s, std::byte* buffer, size_t N)
	{
		auto n = std::min(N, static_cast<size_t>(s.buffer.end() - s.read_ptr));
		memcpy(buffer, s.read_ptr, n);
		s.read_ptr += n;
		return n;
	}
}

namespace Testcases
{
	void maikeConfigInit()
	{
		Maike::Config cfg;
		assert(cfg.env() == Maike::EnvStore{});
		assert(cfg.vcsConfig() == Maike::VcsInvoker::Config{});
		assert(cfg.inputFilter() == Maike::InputFilter{});
		assert(cfg.targetDir() == "%MAIKE_TARGETS%");
	}

	void maikeConfigToJson()
	{
		Maike::Config cfg;
		auto json = toJson(cfg);
		TestSink sink;
		store(json, sink);
		char const* expected = R"_({
    "env": {
        "AR": "ar",
        "AS": "as",
        "CC": "cc",
        "CXX": "g++",
        "LD": "ld",
        "MAIKE_TARGETS": "__targets"
    },
    "input_filter": [
        "^\\.",
        "^__"
    ],
    "target_dir": "%MAIKE_TARGETS%",
    "vcs_config": {
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
    }
})_";
		assert(sink.buffer == expected);
	}

	void maikeConfigFromJson()
	{
		TestSource src{R"_({
    "env": {
        "AR": "ar",
        "AS": "as",
        "CC": "cc",
        "CXX": "g++",
        "LD": "ld",
        "MAIKE_TARGETS": "__targets"
    },
    "input_filter": [
        "xya",
        "blah"
    ],
    "target_dir": "foobar",
    "vcs_config": {
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
    }
})_"};

		auto json = Maike::KeyValueStore::jsonLoad(src, "");

		auto cfg = json.as<Maike::Config>();
		assert(cfg.env() == Maike::EnvStore{});
		assert(cfg.vcsConfig() == Maike::VcsInvoker::Config{});
		assert(cfg.inputFilter() == Maike::InputFilter{});
		assert(cfg.targetDir() == "foobar");
	}
}

int main()
{
	Testcases::maikeConfigInit();
	Testcases::maikeConfigToJson();
	return 0;
}