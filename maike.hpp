//@	{
//@	 "targets":[ {"name":"maike.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include "exceptionhandler.hpp"
#include <cstddef>
#include <cstdint>

namespace Maike
	{
	class Session;
	class DataSink;
	class DataSource;
	class GraphEdgeWriter;
	class ResourceObject;

	template<class T> class Twins;

	void versionPrint(DataSink& sink);

	void init(ExceptionHandler eh);

	void init(DataSink& standard_output,DataSink& standard_error);

	void init(DataSink& standard_output,DataSink& standard_error
		,ExceptionHandler eh);

	void configDump(const Session& maike,DataSink& sink);

	void configAppendDefault(Session& maike);

	void configAppend(Session& maike,DataSource& source);

	void targetsListAll(const Session& maike,DataSink& sink);

	void targetsListLeaf(const Session& maike,DataSink& sink);

	void targetsListExternal(const Session& maike,DataSink& sink);

	void targetCompile(Session& maike,const char* target_name);
	void targetsCompile(Session& maike);

	void targetDump(const Session& maike,ResourceObject& target
		,const char* target_name);
	void targetsDump(const Session& maike,ResourceObject& target);

	void targetsDumpTSVHeader(DataSink& sink);
	void targetDumpTSV(const Session& maike,DataSink& sink
		,const char* target_name);
	void targetsDumpTSV(const Session& maike,DataSink& sink);

	void graphDump(const Session& maike,GraphEdgeWriter& writer);
	void graphDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);

	void graphInvDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);
	}

#endif
