//@	{
//@	 "targets":[ {"name":"maike.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include <cstddef>
#include <cstdint>

namespace Maike
	{
	class Target;
	class Session;
	class TextWriter;
	class DataSink;
	class DataSource;
	class GraphEdgeWriter;
	class ResourceObject;

	template<class T> class Twins;

	void versionPrint(TextWriter& writer);

	void configDump(const Session& maike,DataSink& sink);

	void configLoad(Session& maike,DataSource& source);

	void targetsListAll(const Session& maike,TextWriter& writer);

	void targetsListLeaf(const Session& maike,TextWriter& writer);

	void targetsListExternal(const Session& maike,TextWriter& writer);

	void targetCompile(Session& maike,const char* target_name);
	void targetsCompile(Session& maike);

	void targetDump(const Session& maike,ResourceObject& target
		,const char* target_name);
	void targetsDump(const Session& maike,ResourceObject& target);

	void graphDump(const Session& maike,GraphEdgeWriter& writer);
	void graphDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);
	}

#endif
