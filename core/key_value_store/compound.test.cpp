//@	{
//@	  "targets":[{"name":"compound.test","type":"application","autorun":1}]
//@	 }

#undef NDEBUG

#include "./compound.hpp"
#include "./array.hpp"

#include <cassert>
#include <cstring>

namespace
{
	struct StringViewSource
	{
		explicit StringViewSource(std::string_view v): read_ptr{v.data()}, n_bytes_left{v.size()}
		{
		}
		char const* read_ptr;
		size_t n_bytes_left;
	};


	size_t read(StringViewSource& src, std::byte* buffer, size_t N)
	{
		auto const n = std::min(N, src.n_bytes_left);
		memcpy(buffer, src.read_ptr, n);
		src.read_ptr += n;
		src.n_bytes_left -= n;
		return n;
	}
}


namespace Testcases
{
	void maikeKeyValueStoreCompoundCreate()
	{
		Maike::KeyValueStore::Compound c{};
		assert(c.size() == 0);
	}

	void maikeKeyValueStoreCompoundLoadEmpty()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json(
		)json"},
		                                 ""};
		assert(c.size() == 0);
	}

	void maikeKeyValueStoreCompoundLoadEmpty2()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json({
		})json"},
		                                 ""};
		assert(c.size() == 0);
	}

	void maikeKeyValueStoreCompoundLoad()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json({
		"key 1": 123,
		"key 2": 456
		})json"},
		                                 ""};
		assert(c.size() == 2);
		assert(c.get<json_int_t>("key 1") == 123);
		assert(c.get<json_int_t>("key 2") == 456);
	}

	void maikeKeyValueStoreCompoundLoadSomethingElse()
	{
		try
		{
			Maike::KeyValueStore::Compound c{StringViewSource{R"json([1, 2, 3])json"}, ""};
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeKeyValueStoreCompoundGetNonExistingKey()
	{
		try
		{
			Maike::KeyValueStore::Compound c{StringViewSource{R"json({"foo":"bar"})json"}, ""};
			(void)c.get<char const*>("blah");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeKeyValueStoreCompoundTakeHandle()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json({
		"key 1": 123,
		"key 2": 456
		})json"},
		                                 ""};

		assert(c.size() == 2);

		auto h = c.takeHandle();
		assert(c.size() == 0);
		assert(h.valid());
	}


	void maikeKeyValueStoreCompoundToJson()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json({
		"key 1": 123,
		"key 2": 456
		})json"},
		                                 ""};

		assert(c.size() == 2);

		auto h = toJson(std::move(c));
		assert(c.size() == 0);
		assert(h.valid());
	}

	void maikeKeyValueStoreCompoundSetAndGetValues()
	{
		Maike::KeyValueStore::Compound c;
		c.set("Foo", 123)
		   .set("Bar", 0.125)
		   .set("Subobj", Maike::KeyValueStore::Compound{}.set("Kaka", "hej"));

		assert(c.get<json_int_t>("Foo") == 123);
		assert(c.get<double>("Bar") == 0.125);

		auto subobj = c.get<Maike::KeyValueStore::CompoundRefConst>("Subobj");
		assert(std::string_view{subobj.get<char const*>("Kaka")} == "hej");
	}

	void maikeKeyValueStoreCompoundRefConstWrongType()
	{
		try
		{
			Maike::KeyValueStore::JsonHandle x{123};
			Maike::KeyValueStore::JsonRefConst y{x.handle(), x.source().c_str()};
			Maike::KeyValueStore::CompoundRefConst{y};
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeKeyValueStoreCompoundCombine()
	{
		Maike::KeyValueStore::Compound a;
		a.set("Foo", 123)
		   .set("Bar", 0.125)
		   .set("Subobj", Maike::KeyValueStore::Compound{}.set("Kaka", "hej"))
		   .set("array", Maike::KeyValueStore::Array{}.append(1).append("foo"));

		{
			Maike::KeyValueStore::Compound b;
			b.set("Foo", 124)
			   .set("Bar", 0.5)
			   .set("Subobj", Maike::KeyValueStore::Compound{}.set("Bulle", "nisse"))
			   .set("array",
			        Maike::KeyValueStore::Array{}
			           .append(Maike::KeyValueStore::Compound{}.set("key", "value"))
			           .append(2));

			a |= b;
		}

		assert(a.get<json_int_t>("Foo") == 124);
		assert(a.get<double>("Bar") == 0.5);

		auto subobj = a.get<Maike::KeyValueStore::CompoundRefConst>("Subobj");
		assert(std::string_view{subobj.get<char const*>("Kaka")} == "hej");
		assert(std::string_view{subobj.get<char const*>("Bulle")} == "nisse");

		auto array = a.get<Maike::KeyValueStore::ArrayRefConst>("array");
		auto i = std::begin(array);
		assert((*i).as<json_int_t>() == 1);
		++i;

		assert(std::string_view{(*i).as<char const*>()} == "foo");
		++i;

		{
			auto compound = (*i).as<Maike::KeyValueStore::CompoundRefConst>();
			assert(std::string_view{compound.get<char const*>("key")} == "value");
			++i;
		}

		assert((*i).as<json_int_t>() == 2);
		++i;
	}
}

int main()
{
	Testcases::maikeKeyValueStoreCompoundCreate();
	Testcases::maikeKeyValueStoreCompoundLoadEmpty();
	Testcases::maikeKeyValueStoreCompoundLoadEmpty2();
	Testcases::maikeKeyValueStoreCompoundLoad();
	Testcases::maikeKeyValueStoreCompoundLoadSomethingElse();
	Testcases::maikeKeyValueStoreCompoundGetNonExistingKey();
	Testcases::maikeKeyValueStoreCompoundTakeHandle();
	Testcases::maikeKeyValueStoreCompoundToJson();
	Testcases::maikeKeyValueStoreCompoundSetAndGetValues();
	Testcases::maikeKeyValueStoreCompoundRefConstWrongType();
	Testcases::maikeKeyValueStoreCompoundCombine();
}