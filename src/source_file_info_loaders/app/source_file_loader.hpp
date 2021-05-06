//@	{
//@	 "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	}

#ifndef APP_SOURCEFILELOADER_HPP
#define APP_SOURCEFILELOADER_HPP

#include "src/source_file_info_loaders/loader.hpp"
#include "src/source_file_info_loaders/maikerule/source_file_loader.hpp"

#include "src/key_value_store/array.hpp"

namespace App
{
	class SourceFileLoader: public Maikerule::SourceFileLoader
	{
	public:
		static Maike::Db::Compiler defaultCompiler()
		{
			return Maike::Db::Compiler{
			   "cxx_linker.py",
			   Maike::KeyValueStore::Compound{}.set("cflags", Maike::KeyValueStore::Array{}.append("-g"))};
		}
	};
}

#endif