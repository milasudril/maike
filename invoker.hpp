//@ {
//@ "targets":[ {"name":"invoker.hpp","type":"include"} ]
//@ }

#ifndef MAIKE_INVOKER_H
#define MAIKE_INVOKER_H

#include "twins.hpp"

namespace Maike
	{
	class DataSink;

	class Invoker
		{
		public:
			virtual int run(const char* command,Twins<const char* const*> args
				,DataSink& standard_output,DataSink& standard_error)=0;
			virtual bool newer(const char* file_a,const char* file_b) const=0;
			virtual void mkdir(const char* name)=0;
			virtual void copy(const char* source,const char* dest)=0;
			virtual bool exists(const char* file) const=0;
		};
	}

#endif
