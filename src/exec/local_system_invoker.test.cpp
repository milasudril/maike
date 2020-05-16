//@	{
//@	 "targets":[{"name":"local_system_invoker.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./local_system_invoker.hpp"

#include <unistd.h>
#include <cassert>

#define XSTR(s) STR(s)
#define STR(s) #s

namespace
{
	struct FileDeleter
	{
		void operator()(FILE* file)
		{
			if(file != nullptr) { fclose(file); }
		}
	};
}

namespace Testcases
{
	void maikeLocalSystemInvokerMkdirFailed()
	{
		Maike::LocalSystemInvoker invoker;
		try
		{
			invoker.mkdir("/this directory does not exist/blah");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeLocalSystemInvokerMkdirSucceeded()
	{
		Maike::LocalSystemInvoker invoker;
		auto path = Maike::fs::path{XSTR(MAIKE_TARGET_DIRECTORY)} / XSTR(MAIKE_CURRENT_DIRECTORY)
		            / "__local_system_invoker_test";
		invoker.mkdir(path);

		// Make sure to remove the directory again...
		rmdir(path.c_str());
	}

	void maikeLocalSystemInvokerSaveBufferFailed()
	{
		Maike::LocalSystemInvoker invoker;
		std::string buffer{"Hello, World"};
		try
		{
			invoker.saveBuffer(reinterpret_cast<std::byte const*>(buffer.data()),
			                   buffer.size(),
			                   "/non-existing directory/blah.txt");
			abort();
		}
		catch(...)
		{
		}
	}

	void maikeLocalSystemInvokerSaveBufferSucceeded()
	{
		Maike::LocalSystemInvoker invoker;
		auto path = Maike::fs::path{XSTR(MAIKE_TARGET_DIRECTORY)} / XSTR(MAIKE_CURRENT_DIRECTORY)
		            / "__local_system_invoker_test.txt";
		auto now = std::to_string(::time(nullptr));
		invoker.saveBuffer(reinterpret_cast<std::byte const*>(now.data()), now.size(), path);

		std::unique_ptr<FILE, FileDeleter> output{fopen(path.c_str(), "rb")};
		auto handle = output.get();
		assert(handle != nullptr);
		std::string result;
		result.resize(now.size());
		assert(fread(result.data(), 1, now.size(), handle) == now.size());
		assert(result == now);
	}
}

int main()
{
	Testcases::maikeLocalSystemInvokerMkdirFailed();
	Testcases::maikeLocalSystemInvokerMkdirSucceeded();
	Testcases::maikeLocalSystemInvokerSaveBufferFailed();
	Testcases::maikeLocalSystemInvokerSaveBufferSucceeded();
}