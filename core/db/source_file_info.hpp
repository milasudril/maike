#ifndef MAIKE_DB_SOURCEFILEINFO_HPP
#define MAIKE_DB_SOURCEFILEINFO_HPP

#include "./dependency.hpp"
#include "./target_info.hpp"
#include "./compiler.hpp"
#include "./rebuild_policy.hpp"

#include <vector>

namespace Maike::Db
{
	class SourceFileInfo
	{
	public:
		/**
		 * Allows to create a node without a source file to be scanned.
		 */
		explicit SourceFileInfo(SourceFileOrigin origin, Compiler&& compiler):
		   m_compiler_default{nullptr}, m_compiler{std::move(compiler)}, m_origin{origin}
		{
		}

		explicit SourceFileInfo(std::vector<TargetInfo>&& targets,
		                        std::reference_wrapper<Compiler const> compiler_default,
		                        Compiler&& compiler,
		                        SourceFileOrigin origin,
		                        RebuildPolicy rebuild_policy):
		   m_targets{std::move(targets)},
		   m_compiler_default{&compiler_default.get()},
		   m_compiler{std::move(compiler)},
		   m_origin{origin},
		   m_rebuild_policy{rebuild_policy}
		{
		}

		std::vector<Dependency> const& useDeps() const
		{
			return m_use_deps;
		}

		SourceFileInfo& useDeps(std::vector<Dependency>&& deps)
		{
			m_use_deps = std::move(deps);
			return *this;
		}

		std::vector<Dependency> const& buildDeps() const
		{
			return m_build_deps;
		}

		SourceFileInfo& buildDeps(std::vector<Dependency>&& deps)
		{
			m_build_deps = std::move(deps);
			return *this;
		}

		SourceFileInfo& add(Dependency&& dep)
		{
			m_use_deps.push_back(std::move(dep));
			return *this;
		}

		std::vector<TargetInfo> const& targets() const
		{
			return m_targets;
		}

		Compiler const& compiler() const
		{
			return m_compiler;
		}

		Compiler const& compilerDefault() const
		{
			return *m_compiler_default;
		}

		std::vector<Dependency> const& childTargetsUseDeps() const
		{
			return m_child_targets_use_deps;
		}

		SourceFileInfo& childTargetsUseDeps(std::vector<Dependency>&& deps)
		{
			m_child_targets_use_deps = std::move(deps);
			return *this;
		}

		SourceFileOrigin origin() const
		{
			return m_origin;
		}

		RebuildPolicy rebuildPolicy() const
		{
			return m_rebuild_policy;
		}

	private:
		std::vector<Dependency> m_use_deps;
		std::vector<Dependency> m_build_deps;
		std::vector<Dependency> m_child_targets_use_deps;
		std::vector<TargetInfo> m_targets;
		Compiler const* m_compiler_default;
		Compiler m_compiler;
		SourceFileOrigin m_origin;
		RebuildPolicy m_rebuild_policy;
	};
}

#endif