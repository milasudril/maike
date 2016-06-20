//@ {
//@ "targets":[ {"name":"invoker.hpp","type":"include"} ]
//@ }

#ifndef MAIKE_INVOKER_H
#define MAIKE_INVOKER_H

namespace Maike
	{
	template<class T>
	struct Twins;

	class Invoker
		{
		public:
			virtual void invoke(const char* command,Twins<const char* const*> args)=0;
		};
	}

#endif
