//@	{
//@	 "targets":[ {"name":"targetbase.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"targetbase.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETBASE_HPP
#define MAIKE_TARGETBASE_HPP

#include "target.hpp"
#include "dependency.hpp"
#include "visibility.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	class Dependency;
	class ResourceObject;

	class PRIVATE TargetBase:public Target
		{
		public:
			explicit TargetBase(const char* name,const char* name_src,const char* in_dir
				,const char* root,size_t id);

			explicit TargetBase(const ResourceObject& obj,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			TargetBase& childCounterIncrement() noexcept
				{
				++m_child_counter;
				return *this;
				}

			size_t childCounterGet() const noexcept
				{return m_child_counter;}

			TargetBase& dependencyAdd(Dependency&& dep);

			Twins<const Dependency*> dependencies() const noexcept
				{
				auto ptr=m_dependencies.data();
				return {ptr,ptr + m_dependencies.size()};
				}


			Twins<Dependency*> dependencies() noexcept
				{
				auto ptr=m_dependencies.data();
				return {ptr,ptr + m_dependencies.size()};
				}


			TargetBase& dependencyInverseAdd(Dependency&& dep);

			Twins<const Dependency*> dependenciesInverseGet() const noexcept
				{
				auto ptr=m_deps_inverse.data();
				return {ptr,ptr + m_deps_inverse.size()};
				}

			size_t idGet() const noexcept
				{return m_id;}

			const char* nameGet() const noexcept
				{return m_name.c_str();}

			const char* sourceNameGet() const noexcept
				{return m_source_name.c_str();}

			const char* inDirGet() const noexcept
				{return m_in_dir.c_str();}

			void dump(ResourceObject& target) const;

			void compile(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir);

			double compilationTimeGet() const noexcept
				{return m_compilation_time;}

			size_t lineCountGet() const noexcept
				{return m_loc;}


		private:
			virtual void compileImpl(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir)=0;

			virtual void dumpDetails(ResourceObject& target) const
				{}

			size_t m_child_counter;
			size_t m_id;
			std::string m_name;
			std::string m_source_name;
			std::string m_in_dir;
			std::vector<Dependency> m_dependencies;
			std::vector<Dependency> m_deps_inverse;
			double m_compilation_time;
			size_t m_loc;
		};
	}

#endif

