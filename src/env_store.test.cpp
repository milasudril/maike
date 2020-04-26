//@	{
//@	  "targets":[{"name":"env_store.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./env_store.hpp"

#include <cassert>
#include <cstring>

extern "C" char** environ;

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
	void maikeEnvStoreInit()
	{
		Maike::EnvStore env;
		assert(env.at("AR") == "ar");
		assert(env.at("AS") == "as");
		assert(env.at("CC") == "cc");
		assert(env.at("CXX") == "g++");
		assert(env.at("LD") == "ld");
	}

	void maikeEnvStoreLoadEnviron()
	{
		Maike::EnvStore env;
		env.loadEnviron(environ);
		assert(env.find("PATH") != std::end(env));
	}

	void maikeEnvStoreToJson()
	{
		Maike::EnvStore env;
		auto json = toJson(env);
		TestSink sink;
		store(json, sink);

		char const* expected = R"_({
    "AR": "ar",
    "AS": "as",
    "CC": "cc",
    "CXX": "g++",
    "LD": "ld"
})_";
		assert(sink.buffer == expected);
	}

	void maikeEnvStoreFromJson()
	{
		TestSource foo{R"_({"foo": "bulle", "bar":"kaka"})_"};
		auto json = Maike::KeyValueStore::jsonLoad(foo, "");
		auto env = json.as<Maike::EnvStore>();
		assert(env.at("foo") == "bulle");
		assert(env.at("bar") == "kaka");
	}
}

int main()
{
	Testcases::maikeEnvStoreInit();
	Testcases::maikeEnvStoreLoadEnviron();
	Testcases::maikeEnvStoreToJson();
	Testcases::maikeEnvStoreFromJson();
	return 0;
}