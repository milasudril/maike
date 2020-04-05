//@	{
//@	 "targets":[{"name":"source_reader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_reader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_CXX_SOURCEREADER_HPP
#define MAIKE_CXX_SOURCEREADER_HPP

#include "src/reader.hpp"
#include "src/writer.hpp"
#include "src/source_file_info.hpp"

namespace Maike::Cxx
{
	class SourceReader
	{
	public:
		void run(Reader src, std::vector<Dependency>& deps) const;
	};
}

#endif