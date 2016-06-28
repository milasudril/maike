//@	{
//@	 "targets":[{"name":"targetcxx.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxx.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXX_HPP
#define MAIKE_TARGETCXX_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetCxx:public TargetBase
		{
		public:
			enum class Type:unsigned int
				{INCLUDE,OBJECT,APPLICATION,LIB_DYNAMIC,LIB_STATIC};

			TargetCxx(const char* name,const char* name_src,const char* in_dir
				,size_t id,Type type);
			~TargetCxx();

		private:
			Type m_type;
		};
	}

#endif
