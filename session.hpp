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
#include "targetcxxhook.hpp"
#include "targetpythonhook.hpp"
#include "target_factorydelegatordefault.hpp"

namespace Maike
	{
	class ResourceObject;

	class Session
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
			Session& dependenciesClear() noexcept;
			Session& dependenciesReload();

			void targetsProcess(DependencyGraph::TargetProcessor&& proc);
			void targetsProcess(DependencyGraph::TargetProcessorConst&& proc) const;
			const Target& target(const char* name) const;
			Target& target(const char* name);
			const char* targetDirectoryGet() const noexcept;
			const Twins<size_t>& targetIdRangeGet() const;

		private:
			SystemTargetInfo m_targetinfo;
			TargetDirectoryLoader m_dirloader;
			Handle<TargetCxxHook> m_cxxhook;
			Handle<TargetPythonHook> m_pythonhook;

			ExpressionEvaluator m_evaluator;
			IdGenerator<size_t> m_id_gen;
			Target_FactoryDelegatorDefault m_delegator;
			mutable DependencyGraphDefault m_graph;
			mutable bool m_graph_dirty;
			SpiderDefault m_spider;
			std::vector<std::string> m_source_files;
		};
	}

#endif
