//@	{
//@	 "targets":[ {"name":"dependency.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"dependency.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DEPENDENCY_HPP
#define MAIKE_DEPENDENCY_HPP

#include "target.hpp"
#include <string>

namespace Maike
	{
	class ResourceObject;

	class Dependency
		{
		public:
			enum class Relation:unsigned int
				{
				 LEAF
				,INTERNAL
				,IMPLEMENTATION
				,EXTERNAL
				};

			Dependency():r_target(nullptr),m_rel(Relation::LEAF)
				{}

			explicit Dependency(Target& target):Dependency()
				{r_target=&target;}

			explicit Dependency(Target& target,Relation rel):
				r_target(&target),m_rel(rel)
				{}

			explicit Dependency(const char* name,Relation relation):
				m_name(name),r_target(nullptr),m_rel(relation)
				{}

			explicit Dependency(const ResourceObject& obj);

			explicit Dependency(const ResourceObject& obj,const char* in_dir);

			Dependency(const Dependency&)=default;
            Dependency& operator=(const Dependency&)=default;
            Dependency(Dependency&&)=default;
            Dependency& operator=(Dependency&&)=default;

			const char* nameGet() const noexcept
				{
				return r_target==nullptr?
					m_name.c_str() : r_target->nameGet();
				}

			Dependency& targetSet(Target& target,Target& target_from)
				{
				r_target=&target;
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
			std::string m_name;
			Target* r_target;
			Relation m_rel;
		};
	}

#endif
