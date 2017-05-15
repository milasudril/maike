//@	{"targets":[{"name":"targetplaceholder.hpp","type":"include"}]}

#ifndef MAIKE_TARGETPLACEHOLDER_HPP
#define MAIKE_TARGETPLACEHOLDER_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class PRIVATE TargetPlaceholder final:public Maike::TargetBase
		{
		public:
			static TargetPlaceholder* create(const char* name,const char* name_src
				,const char* root,size_t id,bool file_is)
				{return new TargetPlaceholder(name,name_src,root,id,file_is);}

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

			bool fileIs() const noexcept
				{return m_file_is;}

		private:
			TargetPlaceholder(const char* name,const char* name_src,const char* root,size_t id,bool file_is):
				TargetBase(name,name_src,"",root,id),m_file_is(file_is)
				{}

			~TargetPlaceholder()=default;

			bool m_file_is;
		};
	}

#endif
