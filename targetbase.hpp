//@	{
//@	 "targets":[ {"name":"targetbase.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"name":"targetbase.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETBASE_HPP
#define MAIKE_TARGETBASE_HPP

#include "target.hpp"
#include "dependency.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	class Dependency;
	class Invoker;

	class TargetBase:public Target
		{
		public:
			explicit TargetBase(const char* name,const char* name_src,const char* in_dir,size_t id);

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

			size_t idGet() const noexcept
				{return m_id;}

			const char* nameGet() const noexcept
				{return m_name.c_str();}

			const char* nameFullGet() const noexcept
				{return m_name_full.c_str();}

			const char* sourceNameGet() const noexcept
				{return m_source_name.c_str();}

			const char* sourceNameFullGet() const noexcept
				{return m_source_name_full.c_str();}


		private:
			size_t m_child_counter;
			size_t m_id;
			std::string m_name;
			std::string m_name_full;
			std::string m_source_name;
			std::string m_source_name_full;
			std::vector<Dependency> m_dependencies;
		};
	}

#endif

