//@	{
//@	 "targets":[ {"name":"maike.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include <cstddef>

namespace Maike
	{
	class Target;
	class DataSink;
	template<class T>
	class Twins;

	void versionPrint(DataSink&& sink);
	void versionPrint(DataSink& sink);

	void buildBranch(Target& target,const char* target_dir,const Twins<size_t>& id_range);
	}

#endif
