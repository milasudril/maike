//@	{
//@	 "targets":[ {"name":"fileutils.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"fileutils.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_FILEUTILS_HPP
#define MAIKE_FILEUTILS_HPP


namespace Maike
	{
	class DataSink;

	namespace FileUtils
		{
		bool newer(const char* file_a,const char* file_b);
		void mkdir(const char* name);
		void copy(const char* source,const char* dest);
		bool exists(const char* file);
		};
	}

#endif

