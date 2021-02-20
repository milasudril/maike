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
#include "stringkey.hpp"
#include "dependencybufferdefault.hpp"
#include <map>

namespace Maike
	{
	class ExpressionEvaluator;
	class PRIVATE Target_FactoryDelegatorDefault final:public Target_FactoryDelegator
		,public DependencyGraph::EventHandler
		{
		public:
			explicit Target_FactoryDelegatorDefault(const ExpressionEvaluator& eval);

			Target_FactoryDelegatorDefault(ExpressionEvaluator&& eval)=delete;

			Target_FactoryDelegatorDefault& loaderRegister(const Stringkey& filename_ext
				,const Target_Loader& loader) override;

			void loadersUnregister() noexcept override;

			Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t line_count) override;

			Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir
				,size_t line_count) override;

			void targetsCreate(TagExtractor& extractor,const char* name_src
				,const char* in_dir
				,const Target_Loader& loader_current
				,Spider& spider
				,DependencyGraph& graph) override;

			void targetsCreate(TagExtractor& extractor,const char* in_dir
				,const Target_Loader& loader_current
				,Spider& spider,DependencyGraph& graph) override;

			size_t idGet() noexcept override
				{return m_id_gen.idGet();}

			const char* rootGet() const noexcept override
				{return m_root.c_str();}

			Target_FactoryDelegatorDefault& rootSet(const char* root)
				{
				m_root=std::string(root);
				return *this;
				}

			Target& dependencyResolve(DependencyGraph& graph
				,const char* target_from,const Dependency& dep) override;

			void targetRemoved(DependencyGraph&,Target& target) override
				{m_id_gen.idRelease(target.idGet());}

			void graphCleared(DependencyGraph&) override
				{m_id_gen.reset();}


			void targetsLoad(const char* filename,const char* in_dir,Spider& spider
				,DependencyGraph& targets) override;

			bool loaderHas(const char* filename) const noexcept;

			void depsExtraCacheClear() noexcept
				{m_deps_extra_cache.clear();}

		private:
			void targetsCreateImpl(TagExtractor& extractor,const char* name_src
				,const char* in_dir,const Target_Loader& loader_current
				,Spider& spider,DependencyGraph& graph);

			const ExpressionEvaluator& r_eval;
			IdGenerator<size_t> m_id_gen;
			std::string m_root;
			std::map<Stringkey,const Target_Loader*> m_r_loaders;
			std::map<Stringkey,DependencyBufferDefault> m_deps_extra_cache;
			size_t m_id_current;
		};
	}

#endif
