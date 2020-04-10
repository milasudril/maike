//@	{
//@	  "targets":[{"name":"dependency_extractor.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./dependency_extractor.hpp"

#include <string>
#include <algorithm>
#include <cstring>

namespace
{
	struct DepsExtractor
	{
	};

	void run(DepsExtractor, Maike::Reader reader, std::vector<Maike::Dependency>& output)
	{
		std::array<char, 12> buffer;
		read(reader, reinterpret_cast<std::byte*>(buffer.data()), buffer.size());

		std::string path;
		std::copy(std::begin(buffer), std::end(buffer), std::back_inserter(path));
		output.push_back(Maike::Dependency{Maike::fs::path{path}, Maike::Dependency::Resolver::InternalLookup});
	}

	struct DummyReader
	{};

	size_t read(DummyReader, std::byte* buffer, size_t n)
	{
		assert(n == 12);
		memcpy(buffer, "Hello, World", 12);
		return n;
	}
}

namespace Testcases
{
	void maikeDependencyExtractorCreateAndRun()
	{
		DepsExtractor extractor;
		Maike::DependencyExtractor dep_extractor{extractor};
		DummyReader reader;
		std::vector<Maike::Dependency> deps;
		run(dep_extractor, Maike::Reader{reader}, deps);
		assert(deps[0].name() == "Hello, World");
	}
}

int main()
{
	Testcases::maikeDependencyExtractorCreateAndRun();
	return 0;
}