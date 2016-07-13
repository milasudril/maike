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
	class Session;
	class DataSink;
	class DataSource;

	template<class T> class Twins;

	void versionPrint(DataSink& sink);
	void versionPrint(const char* filename);

	void configDump(const Session& maike,DataSink& sink);
	void configDump(const Session& maike,const char* filename);

	void configLoad(Session& maike,DataSource& source);
	void configLoad(Session& maike,const char* filename);

	void targetsListAll(const Session& session,DataSink& sink);
	void targetsListAll(const Session& session,const char* filename);

	void targetsListLeaf(const Session& session,DataSink& sink);
	void targetsListLeaf(const Session& session,const char*filename);

	void targetsListExternal(const Session& session,DataSink& sink);
	void targetsListExternal(const Session& session,const char* filename);

	void targetCompile(Session& session,const char* target_name);
	void targetsCompile(Session& session);
	}

#endif
