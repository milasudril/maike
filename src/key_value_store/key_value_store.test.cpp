//@	{
//@	  "targets":[{"name":"key_value_store.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./compound.hpp"

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
		auto const n =std::min(N, src.n_bytes_left);
		memcpy(buffer, src.read_ptr, n);
		src.read_ptr+=n;
		src.n_bytes_left-=n;
		return n;
	}

	char const* name(StringViewSource)
	{
		return "<string view>";
	}

	struct StringSink
	{
		std::string buffer;
	};

	struct TestType
	{
		std::string item;
		long long int count;
	};

	void write(StringSink& sink, char const* data, size_t N)
	{
		sink.buffer.insert(std::end(sink.buffer), data, data + N);
	}

	Maike::KeyValueStore::JsonHandle toJson(TestType const& obj)
	{
		return toJson(
		   Maike::KeyValueStore::Compound{}.set("item", obj.item.c_str()).set("count", obj.count));
	}

	TestType fromJson(Maike::KeyValueStore::Empty<TestType>, Maike::KeyValueStore::JsonRefConst obj)
	{
		auto compound = obj.as<Maike::KeyValueStore::CompoundRefConst>();
		return TestType{compound.get<char const*>("item"), compound.get<json_int_t>("count")};
	}

}

namespace Testcases
{
	void maikeKeyValueStoreSetAndGetTestTypeThroughAdl()
	{
		TestType const obj{"Foobar", 123};
		Maike::KeyValueStore::Compound c1{};
		c1.set("foobar", obj);

		StringSink s;
		store(c1, s);

		Maike::KeyValueStore::Compound c2{StringViewSource{s.buffer}};
		auto val = c2.get<TestType>("foobar");
	}
}

int main()
{
	Testcases::maikeKeyValueStoreSetAndGetTestTypeThroughAdl();
	return 0;
}