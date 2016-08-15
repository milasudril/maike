//@	{
//@	 "targets":[{"name":"target_factorydelegatordefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_factorydelegatordefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_FACTORYDELEGATORDEFAULT_HPP
#define MAIKE_TARGET_FACTORYDELEGATORDEFAULT_HPP

#include "target_factorydelegator.hpp"
#include "dependencygraph.hpp"
#include "idgenerator.hpp"
#include "visibility.hpp"
#include "target.hpp"
#include <map>

namespace Maike
	{
	class ExpressionEvaluator;
	class PRIVATE Target_FactoryDelegatorDefault:public Target_FactoryDelegator
		,public DependencyGraph::EventHandler
		{
		public:
			explicit Target_FactoryDelegatorDefault(const ExpressionEvaluator& eval);

			Target_FactoryDelegatorDefault(ExpressionEvaluator&& eval)=delete;

			Target_FactoryDelegatorDefault& factoryRegister(const Stringkey& filename_ext
				,const Target_Factory& factory);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t line_count);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir
				,size_t line_count);

			void targetsCreate(TagExtractor&& extractor,const char* name_src
				,const char* in_dir,DependencyCollector&& cb
				,DependencyGraph& graph);

			void targetsCreate(TagExtractor&& extractor,const char* in_dir
				,DependencyCollector&& cb,DependencyGraph& graph);

			size_t idGet() noexcept
				{return m_id_gen.idGet();}

			void factoriesUnregister() noexcept;

			const char* rootGet() const noexcept
				{return m_root.c_str();}

			Target_FactoryDelegatorDefault& rootSet(const char* root)
				{
				m_root=std::string(root);
				return *this;
				}

			Target& dependencyResolve(DependencyGraph& graph
				,const char* target_from,const Dependency& dep);

			void targetRemoved(DependencyGraph& graph,Target& target)
				{m_id_gen.idRelease(target.idGet());}

			void graphCleared(DependencyGraph& graph)
				{m_id_gen.reset();}

		private:
			void targetsCreateImpl(TagExtractor& extractor,const char* name_src
				,const char* in_dir,DependencyCollector& cb
				,DependencyGraph& graph);

			const ExpressionEvaluator& r_eval;
			IdGenerator<size_t> m_id_gen;
			std::string m_root;
			std::map<Stringkey,const Target_Factory*> m_r_factories;
			size_t m_id_current;
		};
	}

#endif
