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
	{
	};

	size_t read(ReaderStub&, std::byte*, size_t)
	{
		return 0;
	}

	struct WriterStub
	{
	};

	void write(WriterStub&, std::byte const*, size_t)
	{
	}

	struct CompilerStub
	{
		CompilerStub(Maike::KeyValueStore::CompoundRefConst cfg)
		{
			m_command = cfg.get<char const*>("command");
		}

		template<class... Args>
		int run(Args...) const
		{
			return 0;
		}

		template<class Arg>
		void settings(Arg&&)
		{
		}

		Maike::KeyValueStore::Compound settings() const
		{
			return Maike::KeyValueStore::Compound{}.set("command", m_command.c_str());
		}

		std::string m_command;
	};

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

	std::vector<Maike::Dependency> loadDependencies(SourceFileLoaderStub const& stub,
	                                                Maike::Reader input)
	{
		stub.result.get().input = input;
		return std::vector{Maike::Dependency{Maike::fs::path{"foo"}, Maike::Dependency::Resolver::None}};
	}

	Maike::Compiler getCompiler(SourceFileLoaderStub const&,
	                            Maike::KeyValueStore::CompoundRefConst cfg)
	{
		return Maike::Compiler{CompilerStub{cfg}};
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

	void maikeSourceFileLoaderLoadDependencies()
	{
		CallResult res;
		Maike::SourceFileLoader obj{SourceFileLoaderStub{res}};

		ReaderStub input;
		auto deps = obj.loadDependencies(Maike::Reader{input});
		assert(deps.size() == 1);
		assert(deps[0].name() == "foo");
		assert(res.input.identity() == reinterpret_cast<uintptr_t>(&input));
	}


	void maikeSourceFileLoaderGetCompiler()
	{
		CallResult res;
		Maike::SourceFileLoader obj{SourceFileLoaderStub{res}};

		Maike::KeyValueStore::Compound test{};
		test.set("command", "test");
		auto compiler = obj.getCompiler(test.reference());

		assert(compiler.settings().get<char const*>("command") == std::string{"test"});
	}
}

int main()
{
	Testcases::maikeSourceFileLoaderCreateFromObject();
	Testcases::maikeSourceFileLoaderCreateFromUniquePtr();
	Testcases::maikeSourceFileLoaderRunFilter();
	Testcases::maikeSourceFileLoaderLoadDependencies();
	Testcases::maikeSourceFileLoaderGetCompiler();
}