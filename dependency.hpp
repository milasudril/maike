//@	{
//@	 "targets":[ {"name":"dependency.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"dependency.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DEPENDENCY_HPP
#define MAIKE_DEPENDENCY_HPP

#include "target.hpp"
#include "visibility.hpp"
#include "pathutils.hpp"
#include <string>
#include <cassert>

namespace Maike
	{
	class ResourceObject;

	class PRIVATE Dependency
		{
		public:
			enum class Relation:unsigned int
				{
				 LEAF /**<This is the default relation used for empty dependencies only.*/
				,INTERNAL /**<The dependency was synthesized by Maike.*/
				,INCLUDE /**<The dependency was found in a link or an include statement from a Target.*/
				,INCLUDE_EXTRA /**<This is similar to INCLUDE, but the file will passed to the compiler on the command line.*/
				,IMPLEMENTATION /**<The secondary Target should be used when linking or archiving the primary Target.*/
				,EXTERNAL /**<The secondary Target is unknown to Maike*/
				,TOOL /**<This relation is similar to EXTERNAL, but is used to annotate that the secondary Target is a tool*/
				,MISC /**<Any other kind of relation.*/
				,RUNTIME /**<This relation is oly present at runtime, not during compilation.*/
				,EXTERNAL_RESOURCE /**<External resource (package or file) used by scripts.*/
				};

			Dependency():m_name(nullptr),r_target(nullptr),m_rel(Relation::LEAF)
				{}

			explicit Dependency(Target& target):Dependency()
				{r_target=&target;}

			explicit Dependency(Target& target,Relation rel):
				m_name(nullptr),r_target(&target),m_rel(rel)
				{}

			explicit Dependency(const char* name,const char* root,Relation relation):
				m_name(nullptr),r_target(nullptr),m_rel(relation)
				{nameSet(rootStrip(name,root).c_str());}

			explicit Dependency(const char* name,Relation relation):
				m_name(nullptr),r_target(nullptr),m_rel(relation)
				{nameSet(name);}

			Dependency(Dependency&& obj) noexcept
				{
				m_name=obj.m_name;
				r_target=obj.r_target;
				m_rel=obj.m_rel;
				obj.m_name=nullptr;
				}

			Dependency& operator=(Dependency&& obj) noexcept
				{
				std::swap(obj.m_name,m_name);
				r_target=obj.r_target;
				m_rel=obj.m_rel;
				return *this;
				}
			~Dependency() noexcept
				{nameFree();}

			Dependency(const Dependency& obj)
				{
				m_name=nullptr;
				if(obj.m_name==nullptr)
					{
					r_target=obj.r_target;
					}
				else
					{
					nameSet(obj.m_name);
					r_target=nullptr;
					}
				m_rel=obj.m_rel;
				}

            Dependency& operator=(const Dependency& obj) noexcept
				{
				Dependency obj_new(obj);
				*this=std::move(obj_new);
				return *this;
				}

			explicit Dependency(const ResourceObject& obj,const char* in_dir,const char* root);


			const char* nameGet() const noexcept
				{
				return r_target==nullptr?
					m_name : r_target->nameGet();
				}

			Dependency& targetSet(Target& target,Target& target_from)
				{
				r_target=&target;
				nameFree();
				target.dependencyInverseAdd(Dependency(target_from,m_rel));
				return *this;
				}

			Target* target() noexcept
				{return r_target;}

			const Target* target() const noexcept
				{return r_target;}

			Relation relationGet() const noexcept
				{return m_rel;}

			void dump(ResourceObject& dependency) const;

			const char* relationGetStr() const;

		private:
			void nameSet(const char* name);
			void nameSet(const char* name,size_t size);
			void nameFree() noexcept;
			char* m_name;
			Target* r_target;
			Relation m_rel;
		};


	static constexpr int USE_EXTERNAL=1;
	static constexpr int USE_IMPLEMENTATION=2;
	static constexpr int USE_INCLUDE=4;
	static constexpr int USE_INCLUDE_EXTRA=8;
	static constexpr int USE_MISC=16;
	static constexpr int USE_LEAF=32;
	static constexpr int USE_INTERNAL=64;
	static constexpr int USE_TOOL=128;
	static constexpr int USE_ALL=USE_EXTERNAL|USE_IMPLEMENTATION
		|USE_INCLUDE|USE_INCLUDE_EXTRA
		|USE_MISC|USE_LEAF
		|USE_INTERNAL|USE_TOOL;

	static std::string name_full_get(const char* target_dir,const Target& t)
		{
		if(t.generated())
			{return dircat(target_dir,t.nameGet());}
		return t.nameGet();
		}

	template<class Proc>
	static bool dependenciesProcess(const char* target_dir,Twins<const Dependency*> deps
		,int use_flags,Proc&& proc)
		{
		while(deps.first!=deps.second)
			{
			const auto rel=deps.first->relationGet();
			if(rel==Dependency::Relation::EXTERNAL && (use_flags&USE_EXTERNAL) )
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}
			if(rel==Dependency::Relation::IMPLEMENTATION && (use_flags&USE_IMPLEMENTATION) )
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}
			if(rel==Dependency::Relation::INCLUDE && (use_flags&USE_INCLUDE))
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}
			if(rel==Dependency::Relation::INCLUDE_EXTRA && (use_flags&USE_INCLUDE_EXTRA))
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}
			if(rel==Dependency::Relation::TOOL && (use_flags&USE_TOOL))
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}
			if(rel==Dependency::Relation::MISC && (use_flags&USE_MISC))
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}
			if(rel==Dependency::Relation::INTERNAL && (use_flags&USE_INTERNAL))
				{
				auto t=deps.first->target();
				if(!proc(t->nameGet(),rel))
					{return 0;}
				}	
			if(rel==Dependency::Relation::LEAF && (use_flags&USE_LEAF))
				{
				auto name_full=name_full_get(target_dir,*deps.first->target());
				if(!proc(name_full.c_str(),rel))
					{return 0;}
				}	

			++deps.first;
			}
		return 1;
		}
	}

#endif
