//@	{
//@	 "targets":[{"name":"targetexternals.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetexternals.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETEXTERNALS_HPP
#define MAIKE_TARGETEXTERNALS_HPP

#include "target.hpp"
#include "dependency.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace Maike
	{
	class DependencyGraph;

	class PRIVATE TargetExternals final:public Target
		{
		public:
			static TargetExternals* create(DependencyGraph&& graph,size_t id)=delete;

			static TargetExternals* create(const DependencyGraph& graph,size_t id)
				{return new TargetExternals(graph,id);}

			void compile(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) override;

			TargetExternals& dependencyAdd(Dependency&&) override
				{return *this;}

			Twins<const Dependency*> dependencies() const noexcept override
				{return {&m_dep,&m_dep + 1};}

			Twins<Dependency*> dependencies() noexcept override
				{return {&m_dep,&m_dep + 1};}

			TargetExternals& dependencyInverseAdd(Dependency&& dep) override
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
				{return "externals.json";}

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
				{return "File containing all external targets";}

			bool generated() const noexcept override
				{return 1;}

		private:
			explicit TargetExternals(const DependencyGraph& graph,size_t id);
			virtual ~TargetExternals()
				{}
			void destroy() noexcept override
				{delete this;}
			size_t m_id;
			Dependency m_dep;
			std::vector<Dependency> m_deps_inverse;
			mutable double m_compilation_time;
			const DependencyGraph& r_graph;
			mutable std::string m_data;
		};
	}

#endif
