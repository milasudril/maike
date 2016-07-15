//@	{"targets":[{"name":"targetplaceholder.hpp","type":"include"}]}

#ifndef MAIKE_TARGETPLACEHOLDER_HPP
#define MAIKE_TARGETPLACEHOLDER_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetPlaceholder final:public Maike::TargetBase
		{
		public:
			static TargetPlaceholder* create(const char* name,const char* name_src
				,size_t id)
				{return new TargetPlaceholder(name,name_src,id);}

			virtual void destroy() noexcept
				{delete this;}

			virtual bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const
				{return 1;}

			void compileImpl(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir)
				{}

		private:
			TargetPlaceholder(const char* name,const char* name_src,size_t id):
				TargetBase(name,name_src,"",id)
				{}

			~TargetPlaceholder()=default;
		};
	}

#endif
