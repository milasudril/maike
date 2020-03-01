//@	{
//@	 "targets":[{"name":"local_system_invoker.o","type":"object"}]
//@	 }

#include "./local_system_invoker.hpp"

#include <memory>
#include <cstdio>

namespace
{
	struct FileDeleter
	{
		void operator()(FILE* file)
		{
			if(file != nullptr)
			{ fclose(file); }
		}
	};
}

void Maike::LocalSystemInvoker::saveBuffer(std::byte const* buffer, size_t n, fs::path const& file) const
{
	std::unique_ptr<FILE, FileDeleter> output{fopen(file.c_str(), "wb")};
	auto handle = output.get();
	if(handle == nullptr)
	{
		throw std::runtime_error{"Failed to create output file"};
	}

	if(fwrite(buffer, n, 1, handle) != n)
	{
		throw std::runtime_error{"Failed to write all data to the file"};
	}
}

