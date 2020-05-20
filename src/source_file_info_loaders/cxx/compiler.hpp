//@	{
//@	 "targets":[{"name":"compiler.hpp","type":"include"}]
//	,"dependencies_extra":[{"ref":"source_file_loader.o","rel":"implementation"}]
//@	}

#ifndef CXX_COMPILER_HPP
#define CXX_COMPILER_HPP

#include "src/command.hpp"

namespace Cxx
{
	class Compiler
	{
	public:
		Compiler():
			m_cxxversion{"c++17"},
			m_cmd{"%env:CXX%", {"-c",
						"?-std=%cxx:cxxversion%",
						"*-%cxx:cflags%",
						"*-iquote%cxx:localinclude%",
						"*-I%cxx:sysinclude%",
						"-DMAIKE_JOB_ID=%maike:jobid%",
						"-o",
						"%maike:target%",
						"%maike:source%"}}
			{}


	private:
		std::string m_cxxversion;
		std::vector<Maike::fs::path> m_localinclude;
		std::vector<Maike::fs::path> m_sysinclude;
		Maike::Command m_cmd;
	};

#if 0
	class SourceFileLoader
	{
	public:
		std::vector<Maike::Dependency> getDependencies(Maike::Io::Reader src) const;

		void filterInput(Maike::Io::Reader input,
		                 Maike::SourceTreeLoader::SourceOutStream source_stream,
		                 Maike::SourceTreeLoader::TagsOutStream tag_stream) const;

		Maike::Compiler getCompiler(Maike::KeyValueStore::CompoundRefConst) const
		{
			return Maike::Compiler{};
		}

	private:
		Compiler m_compiler;
	};

	inline auto getDependencies(SourceFileLoader const& loader, Maike::Io::Reader src)
	{
		return loader.getDependencies(src);
	}

	inline auto filterInput(SourceFileLoader const& loader,
	                        Maike::Io::Reader input,
	                        Maike::SourceTreeLoader::SourceOutStream source_stream,
	                        Maike::SourceTreeLoader::TagsOutStream tag_stream)
	{
		return loader.filterInput(input, source_stream, tag_stream);
	}

	inline auto getCompiler(SourceFileLoader const& loader, Maike::KeyValueStore::CompoundRefConst cfg)
	{
		return loader.getCompiler(cfg);
	}

#endif
}

#endif