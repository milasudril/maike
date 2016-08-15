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
				 LEAF
				,INTERNAL
				,IMPLEMENTATION
				,EXTERNAL
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

			Dependency(const Dependency& obj) noexcept
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

			explicit Dependency(const ResourceObject& obj,const char* root);

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


		private:
			void nameSet(const char* name);
			void nameSet(const char* name,size_t size);
			void nameFree() noexcept;
			char* m_name;
			Target* r_target;
			Relation m_rel;
		};
	}

#endif
