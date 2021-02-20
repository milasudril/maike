//@	{
//@	 "targets":[{"name":"targetconfig.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetconfig.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCONFIG_HPP
#define MAIKE_TARGETCONFIG_HPP

#include "target.hpp"
#include "dependency.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace Maike
	{
	class PRIVATE TargetConfig final:public Target
		{
		public:
			static TargetConfig* create(const ResourceObject& config,size_t id)
				{return new TargetConfig(config,id);}

			void compile(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) override;

			Target& dependencyAdd(Dependency&&) override
				{return *this;}

			Twins<const Dependency*> dependencies() const noexcept override
				{return {&m_dep,&m_dep + 1};}

			Twins<Dependency*> dependencies() noexcept override
				{return {&m_dep,&m_dep + 1};}

			TargetConfig& dependencyInverseAdd(Dependency&& dep) override
				{
				m_deps_inverse.push_back(std::move(dep));
				return *this;
				}

			Twins<const Dependency*> dependenciesInverseGet() const noexcept override
				{
				return {m_deps_inverse.data(),m_deps_inverse.data() + m_deps_inverse.size()};
				}

			size_t idGet() const noexcept override
				{return m_id;}

			const char* nameGet() const noexcept override
				{return "maikeconfig.json";}

			const char* sourceNameGet() const noexcept override
				{return "";}

			const char* inDirGet() const noexcept override
				{return ".";}

			bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const override;

			void dump(ResourceObject&) const override
				{}

			size_t lineCountGet() const noexcept override
				{return 0;}

			double compilationTimeGet() const noexcept override
				{return m_compilation_time;}

			const char* descriptionGet() const noexcept override
				{return "File containing effective configuration";}

			bool generated() const noexcept override
				{return 1;}

		private:
			explicit TargetConfig(const ResourceObject& config,size_t id);
			virtual ~TargetConfig()
				{}
			void destroy() noexcept override
				{delete this;}
			size_t m_id;
			Dependency m_dep;
			std::vector<Dependency> m_deps_inverse;
			std::string m_content;
			mutable double m_compilation_time;

		};
	}

#endif
