//@	{
//@	 "targets":[{"name":"source_file_loader.o","type":"object"}]
//@	}

#include "./source_file_loader.hpp"

void Maikerule::SourceFileLoader::filterInput(
   Maike::Io::Reader input,
   Maike::SourceFileInfoLoaders::SourceOutStream,
   Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const
{
	std::array<std::byte, 65536> buffer;
	while(true)
	{
		auto n = read(input, buffer.data(), buffer.size());
		if(n == static_cast<size_t>(-1)) { return; }

		if(n == 0) { return; }

		write(tag_stream, buffer.data(), n);
	}
}