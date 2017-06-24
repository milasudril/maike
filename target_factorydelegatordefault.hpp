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

			Target_FactoryDelegatorDefault& factoryRegister(const Stringkey& filename_ext
				,const Target_Factory& factory);

			void factoriesUnregister() noexcept;

			Target_FactoryDelegatorDefault& loaderRegister(const Stringkey& filename_ext
				,const Target_Loader& loader);

			void loadersUnregister() noexcept;

			Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t line_count);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir
				,size_t line_count);

			void targetsCreate(TagExtractor& extractor,const char* name_src
				,const char* in_dir
				,const Target_Loader& loader_current
				,Spider& spider
				,DependencyGraph& graph);

			void targetsCreate(TagExtractor& extractor,const char* in_dir
				,const Target_Loader& loader_current
				,Spider& spider,DependencyGraph& graph);

			size_t idGet() noexcept
				{return m_id_gen.idGet();}

			const char* rootGet() const noexcept
				{return m_root.c_str();}

			Target_FactoryDelegatorDefault& rootSet(const char* root)
				{
				m_root=std::string(root);
				return *this;
				}

			Target& dependencyResolve(DependencyGraph& graph
				,const char* target_from,const Dependency& dep);

			void targetRemoved(DependencyGraph&,Target& target)
				{m_id_gen.idRelease(target.idGet());}

			void graphCleared(DependencyGraph&)
				{m_id_gen.reset();}


			void targetsLoad(const char* filename,const char* in_dir,Spider& spider
				,DependencyGraph& targets);

			bool loaderHas(const Stringkey& filename_ext) const noexcept
				{return m_r_loaders.find(filename_ext)!=m_r_loaders.end();}

			void depsExtraCacheClear() noexcept
				{m_deps_extra_cache.clear();}

		private:
			void targetsCreateImpl(TagExtractor& extractor,const char* name_src
				,const char* in_dir,const Target_Loader& loader_current
				,Spider& spider,DependencyGraph& graph);

			const ExpressionEvaluator& r_eval;
			IdGenerator<size_t> m_id_gen;
			std::string m_root;
			std::map<Stringkey,const Target_Factory*> m_r_factories;
			std::map<Stringkey,const Target_Loader*> m_r_loaders;
			std::map<Stringkey,DependencyBufferDefault> m_deps_extra_cache;
			size_t m_id_current;
		};
	}

#endif
