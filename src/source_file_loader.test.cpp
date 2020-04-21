//@	{
//@	  "targets":[{"name":"source_file_loader.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./source_file_loader.hpp"

#include "src/test_loader/source_file_loader.hpp"

#include <cassert>

namespace
{
	struct CallResult
	{
		Maike::Reader input{};
		Maike::SourceOutStream source{};
		Maike::TagsOutStream tags{};
	};

	struct ReaderStub
	{};

	size_t read(ReaderStub&, std::byte*, size_t)
	{ return 0; }

	struct WriterStub
	{};

	void write(WriterStub&, std::byte const*, size_t)
	{ }

	struct SourceFileLoaderStub
	{
		explicit SourceFileLoaderStub(std::reference_wrapper<CallResult> res): result{res}
		{
		}

		std::reference_wrapper<CallResult> result;
	};

	void filterInput(SourceFileLoaderStub const& stub,
	                 Maike::Reader input,
	                 Maike::SourceOutStream source,
	                 Maike::TagsOutStream tags)
	{
		stub.result.get().input = input;
		stub.result.get().source = source;
		stub.result.get().tags = tags;
	}

	std::vector<Maike::Dependency> loadDependencies(SourceFileLoaderStub const&, Maike::Reader)
	{
		return std::vector<Maike::Dependency>{};
	}

	Maike::Compiler getCompiler(SourceFileLoaderStub const&, Maike::KeyValueStore::CompoundRefConst)
	{
		return Maike::Compiler{};
	}
}

namespace Testcases
{
	void maikeSourceFileLoaderCreateFromObject()
	{
		CallResult res;
		Maike::SourceFileLoader obj{SourceFileLoaderStub{res}};
		assert(obj.valid());
	}

	void maikeSourceFileLoaderCreateFromUniquePtr()
	{
		CallResult res;
		auto tmp = std::make_unique<SourceFileLoaderStub>(std::ref(res));
		Maike::SourceFileLoader obj{std::move(tmp)};
		assert(obj.valid());
		assert(tmp.get() == nullptr);
	}

	void maikeSourceFileLoaderRunFilter()
	{
		CallResult res;
		Maike::SourceFileLoader obj{SourceFileLoaderStub{res}};
		assert(obj.valid());

		ReaderStub input;
		WriterStub source;
		WriterStub tags;
		obj.filterInput(Maike::Reader{input}, Maike::SourceOutStream{source}, Maike::TagsOutStream{tags});

		assert(res.input.identity() == reinterpret_cast<uintptr_t>(&input));
		assert(res.source.identity() == reinterpret_cast<uintptr_t>(&source));
		assert(res.tags.identity() == reinterpret_cast<uintptr_t>(&tags));
	}
}

int main()
{
	Testcases::maikeSourceFileLoaderCreateFromObject();
	Testcases::maikeSourceFileLoaderCreateFromUniquePtr();
	Testcases::maikeSourceFileLoaderRunFilter();
}