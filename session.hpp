//@	{
//@	 "targets":[{"name":"session.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"session.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SESSION_HPP
#define MAIKE_SESSION_HPP

#include "systemtargetinfo.hpp"
#include "targetdirectoryloader.hpp"
#include "handle.hpp"
#include "expressionevaluator.hpp"
#include "idgenerator.hpp"
#include "spiderdefault.hpp"
#include "dependencygraphdefault.hpp"
#include "target_hook_registry.hpp"
#include "target_factorydelegatordefault.hpp"

namespace Maike
	{
	class ResourceObject;

	class PRIVATE Session
		{
		public:
			Session();

			Session& configClear();
			Session& sysvarsLoad();
			Session& configAppendDefault();
			Session& configAppend(const ResourceObject& maikeconfig);
			Session& sourceFileAppend(const char* filename);
			void configDump(ResourceObject& maikeconfig) const;

			Session& scanFile(const char* filename);
			void dependenciesClear() noexcept;

			void targetsProcess(DependencyGraph::TargetProcessor&& proc);
			void targetsProcess(DependencyGraph::TargetProcessorConst&& proc) const;
			const Target& target(const char* name) const;
			Target& target(const char* name);
			const char* targetDirectoryGet() const noexcept;
			const Twins<size_t>& targetIdRangeGet() const;

		private:
			SystemTargetInfo m_targetinfo;
			TargetDirectoryLoader m_dirloader;
			Target_Hook_Registry m_target_hooks;

			ExpressionEvaluator m_evaluator;
			IdGenerator<size_t> m_id_gen;
			std::vector<std::string> m_source_files;
			mutable SpiderDefault m_spider;
			mutable Target_FactoryDelegatorDefault m_delegator;
			mutable DependencyGraphDefault m_graph;
			mutable unsigned int m_dirty_flags;

			static constexpr unsigned int GRAPH_DIRTY=1;
			static constexpr unsigned int TARGET_HOOKS_DIRTY=2;

			inline bool graphDirty() const noexcept
				{return m_dirty_flags&GRAPH_DIRTY;}
			inline void graphDirtySet() const noexcept
				{m_dirty_flags|=GRAPH_DIRTY;}
			inline void graphDirtyClear() const noexcept
				{m_dirty_flags&=~GRAPH_DIRTY;}

			inline bool targetHooksDirty() const noexcept
				{return m_dirty_flags&TARGET_HOOKS_DIRTY;}
			inline void targetHooksDirtySet() const noexcept
				{m_dirty_flags|=TARGET_HOOKS_DIRTY;}
			inline void targetHooksDirtyClear() const noexcept
				{m_dirty_flags&=~TARGET_HOOKS_DIRTY;}

			void targetHooksRegister() const;
			void dependenciesReload() const;
		};
	}

#endif
