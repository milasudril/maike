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

			Dependency();

			explicit Dependency(Target& target):Dependency()
				{r_target=&target;}

			explicit Dependency(const char* name,Relation relation);

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

			Dependency& targetSet(Target& target) noexcept
				{
				r_target=&target;
				target.childCounterIncrement();
				return *this;
				}

			Target* target() noexcept
				{return r_target;}

			const Target* target() const noexcept
				{return r_target;}

			Relation relationGet() const noexcept
				{return m_rel;}



		private:
			std::string m_name;
			Target* r_target;
			Relation m_rel;
		};
	}

#endif
