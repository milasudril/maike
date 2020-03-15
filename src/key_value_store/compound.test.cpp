//@	{
//@	  "targets":[{"name":"compound.test","type":"application","autorun":1}]
//@	 }

#undef NDEBUG

#include "./compound.hpp"

#include <cassert>

struct StringViewSource
{
	explicit StringViewSource(std::string_view v): read_ptr{v.data()}, n_bytes_left{v.size()}
	{
	}
	char const* read_ptr;
	size_t n_bytes_left;
};

int getchar(StringViewSource& src)
{
	if(src.n_bytes_left == 0) { return -1; }

	auto ret = *src.read_ptr;
	++src.read_ptr;
	--src.n_bytes_left;
	return ret;
}


char const* name(StringViewSource)
{
	return "<input buffer>";
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
		)json"}};
		assert(c.size() == 0);
	}

	void maikeKeyValueStoreCompoundLoadEmpty2()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json({
		})json"}};
		assert(c.size() == 0);
	}

	void maikeKeyValueStoreCompoundLoad()
	{
		Maike::KeyValueStore::Compound c{StringViewSource{R"json({
		"key 1": 123,
		"key 2": 456
		})json"}};
		assert(c.size() == 2);
		assert(c.get<json_int_t>("key 1") == 123);
		assert(c.get<json_int_t>("key 2") == 456);
	}

	void maikeKeyValueStoreCompoundLoadSomethingElse()
	{
		try
		{
			Maike::KeyValueStore::Compound c{StringViewSource{R"json([1, 2, 3])json"}};
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
			Maike::KeyValueStore::Compound c{StringViewSource{R"json({"foo":"bar"})json"}};
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
		})json"}};

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
		})json"}};

		assert(c.size() == 2);

		auto h = toJson(std::move(c));
		assert(c.size() == 0);
		assert(h.valid());
	}

	void maikeKeyValueStoreCompoundSetValues()
	{
		Maike::KeyValueStore::Compound c;
		c.set("Foo", 123)
		 .set("Bar", 0.125)
		 .set("Subobj", Maike::KeyValueStore::Compound{}.set("Kaka", "hej"));
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
	Testcases::maikeKeyValueStoreCompoundSetValues();
}