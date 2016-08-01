//@	{
//@	 "targets":[ {"name":"maike_static.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include "exceptionhandler.hpp"
#include "twins.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>

/**\brief The main namespace
*/
namespace Maike
	{
	/**\brief A Session is an opaque type that maintains the dependency graph,
	* and configuration data.
	*/
	class Session;
	class DataSink;
	class DataSource;
	class GraphEdgeWriter;
	class ResourceObject;

	/**\brief Creates a new Maike Session.
	*
	* This function creates a new Maike Session.
	*
	* \note The caller is resposible for deleting the session by calling
	* sessionDestroy.
	*/
	EXPORT Session* sessionCreateRaw();

	/**\brief Destroys a Maike Session.
	*
	* This function destroys the Session pointed to by <var>maike</var>.
	*/
	EXPORT void sessionDestroy(Session* maike);

	/**\brief RAII wrapper for Session lifetime managment.
	* 
	* \see sessionCreateRaw, sessionDestroy(Session*)
	* 
	*/
	PRIVATE inline std::unique_ptr<Session,decltype(&sessionDestroy)>
	sessionCreate()
		{
		return {sessionCreateRaw(),sessionDestroy};
		}

	/**\brief Prints version information to <var>sink</var>.
	* 
	* This function prints version information to <var>sink</var>.
	*/
	EXPORT void versionPrint(DataSink& sink);

	/**\brief Sets an ExceptionHandler that is called when an exception
	* occurs.
	* 
	* This function sets an ExceptionHandler that is called when an exception
	* occurs.
	* 
	*/
	EXPORT void init(ExceptionHandler eh);

	/**\brief This function sets DataSinks for standard output and standard error.
	* 
	* This function sets DataSinks for standard output and standard error.
	* These sinks are used to redirect output data from the programs invoked
	* by Maike during the compilation process. By default, data are written
	* to the corresponding standard stream. Through this function, it is possible
	* to change that behaviour. This is useful when Maike is used from within
	* an IDE, or another GUI application.
	*
	*/
	EXPORT void init(DataSink& standard_output,DataSink& standard_error);

	/**\brief Combines the effect of init(ExceptionHandler), and
	* init(DataSink& standard_output,DataSink& standard_error)
	*
	* This function combines the effect of init(ExceptionHandler), and
	* init(DataSink& standard_output,DataSink& standard_error)
	*
	*/
	EXPORT void init(DataSink& standard_output,DataSink& standard_error
		,ExceptionHandler eh);



	EXPORT void configDump(const Session& maike,DataSink& sink);

	EXPORT void configDump(const Session& maike,ResourceObject& obj);

	EXPORT void configAppendDefault(Session& maike);

	EXPORT void configAppend(Session& maike,DataSource& source);

	EXPORT void configAppend(Session& maike,const ResourceObject& obj);

	EXPORT void configClear(Session& maike);

	EXPORT void sysvarsLoad(Session& maike);


	/**\brief Retrievs the range of target id:s.
	*
	* This function retrieves the range of target id:s. The number of
	* targets is the difference between the two members of the Twins
	* returned.
	*/
	EXPORT Twins<size_t> targetIdRangeGet(const Session& maike);

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

	/**\brief Dumps the dependency graph from and including <var>target_start</var>
	* in forwards.
	*
	* Dumps the dependency graph from and including <var>target_start</var>
	* in forward direction. The parameter <var>targets_visited</var> must
	* point to an array with the same capacity as the range of target id:s.
	* This range, and <var>id_min</var>, can be retrieved
	* by calling targetIdRangeGet(const Session&).
	*
	*/
	EXPORT void graphDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);

	/**\brief Dumps the dependency graph from and including <var>target_start</var>
	* backwards.
	*
	* Dumps the dependency graph from and including <var>target_start</var>
	* in forward direction. The parameter <var>targets_visited</var> must
	* point to an array with the same capacity as the range of target id:s.
	* This range, and <var>id_min</var>, can be retrieved
	* by calling targetIdRangeGet(const Session&).
	*
	*/
	EXPORT void graphInvDump(const Session& maike,GraphEdgeWriter& writer
		,const char* target_start,uint8_t* targets_visited,size_t id_min);
	}

#endif
