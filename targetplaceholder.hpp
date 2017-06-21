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
				,const char* root,size_t id,Dependency::Relation rel)
				{return new TargetPlaceholder(name,name_src,root,id,rel);}

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

			Dependency::Relation relation() const noexcept
				{return m_relation;}

			bool generated() const noexcept
				{return 0;}

		private:
			TargetPlaceholder(const char* name,const char* name_src,const char* root,size_t id
				,Dependency::Relation rel):
				TargetBase(name,name_src,"",root,id),m_relation(rel)
				{}

			~TargetPlaceholder()=default;

			Dependency::Relation m_relation;
		};
	}

#endif
