//@	{
//@	 "targets":[{"name":"session.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"session.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SESSION_HPP
#define MAIKE_SESSION_HPP

#include "systemtargetinfo.hpp"
#include "targetdirectoryloader.hpp"
#include "handle.hpp"
#include "expressionevaluatordefault.hpp"
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
			Session(const ResourceObject& maikeconfig);
			Session& configClear() noexcept;
			Session& configAppend(const ResourceObject& maikeconfig);
			Session& sourceFileAppend(const char* filename);
			void configDumpJSON(DataSink& sink) const;

			Session& scanFile(const char* filename);
			Session& dependenciesClear() noexcept;
			Session& dependenciesReload();

			Session& targetCompile(const char* name);
			Session& targetsCompile();

			void graphDumpDOT(DataSink& sink) const;
			void graphDumpJSON(DataSink& sink) const;
			void targetsDumpCSV(DataSink& sink) const;

		private:
			SystemTargetInfo m_targetinfo;
			TargetDirectoryLoader m_dirloader;
			Handle<TargetCxxHook> m_cxxhook;
			Handle<TargetPythonHook> m_pythonhook;

			ExpressionEvaluatorDefault m_evaluator;
			IdGenerator<size_t> m_id_gen;
			Target_FactoryDelegatorDefault m_delegator;
			mutable DependencyGraphDefault m_graph;
			mutable bool m_graph_dirty;
			SpiderDefault m_spider;
			std::vector<std::string> m_source_files;
		};
	}

#endif
