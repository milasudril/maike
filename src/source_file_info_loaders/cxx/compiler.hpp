//@	{
//@	 "targets":[{"name":"compiler.hpp","type":"include"}]
//	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_COMPILER_HPP
#define CXX_COMPILER_HPP

#include "./stdversion.hpp"

#include "src/command.hpp"

#include "src/compilation_log.hpp"
#include "src/utils/fs.hpp"

namespace Cxx
{
	class Compiler
	{
	public:
		static Maike::fs::path defaultRecipe()
		{
			return Maike::fs::path{"cxx.py"};
		}

		Compiler()
		{
			m_localinclude.push_back(Maike::fs::path{"."});
		}

		explicit Compiler(Maike::KeyValueStore::CompoundRefConst settings):
		   m_stdversion_min{settings.get<Stdversion>("stdversion_min")},
		   m_stdversion_max{settings.get<Stdversion>("stdversion_max")}
		{
			auto from_json_array = [](Maike::KeyValueStore::ArrayRefConst array) {
				std::vector<Maike::fs::path> ret;
				ret.reserve(std::size(array));
				std::transform(std::begin(array),
				               std::end(array),
				               std::back_inserter(ret),
				               [](auto const& item) { return item.template as<char const*>(); });
				return ret;
			};
			m_localinclude =
			   from_json_array(settings.get<Maike::KeyValueStore::ArrayRefConst>("localinclude"));
			m_sysinclude = from_json_array(settings.get<Maike::KeyValueStore::ArrayRefConst>("sysinclude"));
		}

		int run(Maike::fs::path const& src,
		        std::vector<Maike::fs::path const*> const& used_files,
		        std::vector<Maike::fs::path const*> const& output_files,
		        Maike::CompilationLog& log) const
		{
			(void)src;
			(void)used_files;
			(void)output_files;
			(void)log;
			return 0;
		}

		Stdversion stdversionMin() const
		{
			return m_stdversion_min;
		}

		Stdversion stdversionMax() const
		{
			return m_stdversion_max;
		}

		std::vector<Maike::fs::path> const& localinclude() const
		{
			return m_localinclude;
		}

		std::vector<Maike::fs::path> const& sysinclude() const
		{
			return m_sysinclude;
		}

	private:
		Stdversion m_stdversion_min;
		Stdversion m_stdversion_max;
		std::vector<Maike::fs::path> m_localinclude;
		std::vector<Maike::fs::path> m_sysinclude;
	};

	inline auto fromJson(Maike::KeyValueStore::Empty<Compiler>, Maike::KeyValueStore::JsonRefConst ref)
	{
		return Compiler{ref.as<Maike::KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Compiler const& compiler)
	{
		auto to_json_array = [](auto const& seq) {
			return Maike::KeyValueStore::Array{std::begin(seq), std::end(seq)};
		};

		return Maike::KeyValueStore::Compound{}
		   .set("stdversion_min", compiler.stdversionMin())
		   .set("stdversion_max", compiler.stdversionMax())
		   .set("localinclude", to_json_array(compiler.localinclude()))
		   .set("sysinclude", to_json_array(compiler.sysinclude()))
		   .takeHandle();
	}

}

#endif