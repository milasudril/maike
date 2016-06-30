//@	{
//@	 "targets":[{"name":"targetcxx.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxx.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXX_HPP
#define MAIKE_TARGETCXX_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetCxx final:public TargetBase
		{
		public:
			enum class Type:unsigned int
				{INCLUDE,OBJECT,APPLICATION,LIB_DYNAMIC,LIB_STATIC};

			static TargetCxx* create(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t id);

			void compile(Twins<const Dependency*> dependency_list
				,Invoker& invoker,const char* target_dir);

			bool upToDate(Twins<const Dependency*> dependency_list
				,Invoker& invoker,const char* target_dir) const;

			void destroy() noexcept;

		private:
			TargetCxx(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t id);
			Type m_type;
		};
	}

#endif
