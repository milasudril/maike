//@	{"target":{"name":"source_file_loader.test"}}

#include "./source_file_loader.hpp"

#include <cstring>
#include <cassert>
#include <algorithm>

namespace
{
	struct MyReader
	{
	};

	size_t read(MyReader const&, std::byte* buffer, size_t n)
	{
		memset(buffer, 65, n);
		return n;
	}

	struct MyWriter
	{
		std::string buffer;
		size_t offset{0};
	};

	size_t write(MyWriter& writer, std::byte const* buffer, size_t n)
	{
		std::copy_n(reinterpret_cast<char const*>(buffer), n, std::back_inserter(writer.buffer));
		return n;
	}

	size_t read(MyWriter& src, std::byte* buffer, size_t n)
	{
		auto const n_read = std::min(n, static_cast<size_t>(std::size(src.buffer) - src.offset));
		memcpy(buffer, std::data(src.buffer) + src.offset, n_read);
		src.offset += n_read;
		return n;
	}
}

namespace Testcases
{
	void maikeGenericSourceFileLoaderGetDependencies()
	{
		Generic::SourceFileLoader sut;
		MyReader reader;
		auto deps = getDependencies(sut, Maike::Io::Reader{reader});
		assert(std::size(deps) == 0);
	}

	void maikeGenericSourceFileLoaderAppendMode()
	{
		Generic::SourceFileLoader sut{".foo", Generic::FilenameExtReplacementMode::Append};

		assert(sut.targetFilenameExt() == ".foo");
		assert(sut.filenameExtReplacementMode() == Generic::FilenameExtReplacementMode::Append);

		{
			auto json = toJson(sut);
			auto obj = json.as<Maike::KeyValueStore::CompoundRefConst>();
			assert(strcmp(obj.get<char const*>("target_filename_ext"), ".foo") == 0);
			assert(strcmp(obj.get<char const*>("filename_ext_replacement_mode"), "append") == 0);
		}

		{
			MyReader reader;
			MyWriter src_buffer;
			MyWriter tag_buffer;
			Maike::SourceFileInfoLoaders::SourceOutStream source{src_buffer};
			Maike::SourceFileInfoLoaders::TagsOutStream tags{tag_buffer};
			filterInput(sut, Maike::Io::Reader{reader, "source.test"}, source, tags);
			assert(std::size(src_buffer.buffer) == 0);

			assert(std::size(tag_buffer.buffer) != 0);
			auto json = Maike::KeyValueStore::jsonLoad(tag_buffer, "");
			auto obj = json.as<Maike::KeyValueStore::CompoundRefConst>();
			auto target = obj.get<Maike::KeyValueStore::CompoundRefConst>("target");
			assert(strcmp(target.get<char const*>("name"), "source.test.foo") == 0);
		}
	}

	void maikeGenericFilenameExtReplacementModeFromString()
	{
		using Tag = Maike::KeyValueStore::Empty<Generic::FilenameExtReplacementMode>;
		auto a = fromString(Tag{}, "append");
		assert(a == Generic::FilenameExtReplacementMode::Append);

		auto b = fromString(Tag{}, "replace");
		assert(b == Generic::FilenameExtReplacementMode::Replace);

		try
		{
			(void)fromString(Tag{}, "blah");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeGenericFilenameExtReplacementModeToString()
	{
		auto a = toString(Generic::FilenameExtReplacementMode::Append);
		assert(strcmp(a, "append") == 0);

		auto b = toString(Generic::FilenameExtReplacementMode::Replace);
		assert(strcmp(b, "replace") == 0);
	}

	void maikeGenericFilenameExtReplacementModeMakeTargetName()
	{
		auto a = makeTargetName("source.test", ".foo", Generic::FilenameExtReplacementMode::Append);
		assert(a == "source.test.foo");

		auto b = makeTargetName("source.test", ".foo", Generic::FilenameExtReplacementMode::Replace);
		assert(b == "source.foo");

		auto c = makeTargetName("source.test.bar", ".foo", Generic::FilenameExtReplacementMode::Append);
		assert(c == "source.test.bar.foo");

		auto d = makeTargetName("source.test.bar", ".foo", Generic::FilenameExtReplacementMode::Replace);
		assert(d == "source.test.foo");
	}
}

int main()
{
	Testcases::maikeGenericSourceFileLoaderGetDependencies();
	Testcases::maikeGenericSourceFileLoaderAppendMode();
	Testcases::maikeGenericFilenameExtReplacementModeFromString();
	Testcases::maikeGenericFilenameExtReplacementModeToString();
	Testcases::maikeGenericFilenameExtReplacementModeMakeTargetName();
	return 0;
}