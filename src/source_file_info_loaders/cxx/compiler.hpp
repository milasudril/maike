//@	{
//@	 "targets":[{"name":"compiler.hpp","type":"include"}]
//	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_COMPILER_HPP
#define CXX_COMPILER_HPP

#include "src/command.hpp"

#include "src/compilation_log.hpp"

namespace Cxx
{
	class Compiler
	{
	public:
		Compiler():
		   m_cxxversion_min{-1},
		   m_cxxversion_max{-1},
		   m_cmd{"%env:CXX%",
		         {"-c",
		          "?-std=%cxx:cxxversion%",
		          "*-%cxx:cflags%",
		          "*-iquote%cxx:localinclude%",
		          "*-I%cxx:sysinclude%",
		          "-DMAIKE_JOB_ID=%maike:jobid%",
		          "-o",
		          "%maike:target%",
		          "%maike:source%"}}
		{
		}

		Compiler(Maike::KeyValueStore::CompoundRefConst settings):
		   m_cxxversion_min{settings.get<json_int_t>("cxxversion_min")},
		   m_cxxversion_max{settings.get<json_int_t>("cxxversion_max")},
		   m_cmd{settings.get<Maike::Command>("command")}
		{
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

		Maike::KeyValueStore::Compound settings() const
		{
			return Maike::KeyValueStore::Compound{}
			   .set("command", m_cmd)
			   .set("cxxversion_min", m_cxxversion_min)
			   .set("cxxversion_max", m_cxxversion_min);
		}

		void settings(Maike::KeyValueStore::CompoundRefConst settings)
		{
			m_cmd = settings.get<Maike::Command>("command");
			m_cxxversion_max = std::min(m_cxxversion_max, settings.get<json_int_t>("cxxversion_max"));
			m_cxxversion_min = std::max(m_cxxversion_min, settings.get<json_int_t>("cxxversion_min"));
		}


	private:
		json_int_t m_cxxversion_min;
		json_int_t m_cxxversion_max;
		std::vector<Maike::fs::path> m_localinclude;
		std::vector<Maike::fs::path> m_sysinclude;
		Maike::Command m_cmd;
	};

	inline auto fromJson(Maike::KeyValueStore::Empty<Compiler>, Maike::KeyValueStore::JsonRefConst ref)
	{
		return Compiler{ref.as<Maike::KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Compiler const& compiler)
	{
		return compiler.settings().takeHandle();
	}

}

#endif