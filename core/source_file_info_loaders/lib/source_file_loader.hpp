#ifndef LIB_SOURCEFILELOADER_HPP
#define LIB_SOURCEFILELOADER_HPP

#include "core/source_file_info_loaders/loader.hpp"
#include "core/source_file_info_loaders/maikerule/source_file_loader.hpp"

#include "core/key_value_store/array.hpp"

namespace Lib
{
	class SourceFileLoader: public Maikerule::SourceFileLoader
	{
	public:
		static Maike::Db::Compiler defaultCompiler()
		{
			return Maike::Db::Compiler{"make_lib.py", Maike::KeyValueStore::Compound{}, false};
		}

		static char const* name()
		{
			return "lib";
		}
	};

	inline Maike::KeyValueStore::JsonHandle toJson(SourceFileLoader const&)
	{
		return Maike::KeyValueStore::Compound{}.takeHandle();
	}

	inline constexpr bool useTargetDeps(SourceFileLoader const&, Maike::Db::Compiler const&)
	{
		return true;
	}
}

#endif
