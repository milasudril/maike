//@	{"targets":[{"name":"targetplaceholder.hpp","type":"include"}]}

#ifndef MAIKE_TARGETPLACEHOLDER_HPP
#define MAIKE_TARGETPLACEHOLDER_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetPlaceholder final : public Maike::TargetBase
		{
		public:
			TargetPlaceholder(const char* name,const char* name_src
				,const char* in_dir,size_t id):
				TargetBase(name,name_src,in_dir,id)
				{}



			virtual void destroy() noexcept
				{delete this;}

			virtual bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const
				{return 1;}

			void compile(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir)
				{}

		private:
			~TargetPlaceholder()=default;
		};
	}

#endif
