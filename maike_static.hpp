//@	{
//@	 "targets":[ {"name":"maike_static.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include "exceptionhandler.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace Maike
	{
	class Session;
	class DataSink;
	class DataSource;
	class GraphEdgeWriter;
	class ResourceObject;

	template<class T> class Twins;

	EXPORT Session* sessionCreateRaw();

	EXPORT void sessionDestroy(Session* maike);

	PRIVATE inline std::unique_ptr<Session,decltype(&sessionDestroy)>
	sessionCreate()
		{
		return {sessionCreateRaw(),sessionDestroy};
		}


	EXPORT void versionPrint(DataSink& sink);

	EXPORT void init(ExceptionHandler eh);

	EXPORT void init(DataSink& standard_output,DataSink& standard_error);

	EXPORT void init(DataSink& standard_output,DataSink& standard_error
		,ExceptionHandler eh);



	EXPORT void configDump(const Session& maike,DataSink& sink);

	EXPORT void configDump(const Session& maike,ResourceObject& obj);

	EXPORT void configAppendDefault(Session& maike);

	EXPORT void configAppend(Session& maike,DataSource& source);

	EXPORT void configAppend(Session& maike,const ResourceObject& obj);

	EXPORT void configClear(Session& maike);

	EXPORT void sysvarsLoad(Session& maike);

	EXPORT Twins<size_t> targetIdRangeGet(Session& maike);



	EXPORT void targetsListAll(const Session& maike,DataSink& sink);

	EXPORT void targetsListLeaf(const Session& maike,DataSink& sink);

	EXPORT void targetsListExternal(const Session& maike,DataSink& sink);

	EXPORT void targetCompile(Session& maike,const char* target_name);
	EXPORT void targetsCompile(Session& maike);

	EXPORT void targetDump(const Session& maike,ResourceObject& target
		,const char* target_name);
	EXPORT void targetsDump(const Session& maike,ResourceObject& target);

	EXPORT void targetsDumpTSVHeader(DataSink& sink);
	EXPORT void targetDumpTSV(const Session& maike,DataSink& sink
		,const char* target_name);
	EXPORT void targetsDumpTSV(const Session& maike,DataSink& sink);

	EXPORT void graphDump(const Session& maike,GraphEdgeWriter& writer);
	EXPORT void graphDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);

	EXPORT void graphInvDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);
	}

#endif
