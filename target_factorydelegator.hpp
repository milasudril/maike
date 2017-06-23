//@	{"targets":[{"name":"target_factorydelegator.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_FACTORYDELEGATOR_HPP
#define MAIKE_TARGET_FACTORYDELEGATOR_HPP

#include "handle.hpp"
#include "resourceobject.hpp"
#include "datasource.hpp"

namespace Maike
	{
	class Target;
	class Target_Factory;
	class Target_Loader;
	class Stringkey;
	class ResourceObject;
	class Dependency;
	class DependencyGraph;
	class Spider;

	class PRIVATE Target_FactoryDelegator
		{
		public:
			class TagExtractor:public DataSource
				{
				public:
					virtual size_t linesCountGet() const noexcept=0;
				};

			virtual Handle<Target> targetCreate(const ResourceObject& target
				,const char* name_src,const char* in_dir,size_t line_count)=0;

			virtual Handle<Target> targetCreate(const ResourceObject& target
				,const char* in_dir,size_t line_count)=0;

			class DependencyCollector
				{
				public:
					virtual bool operator()(const Target_FactoryDelegator&,Dependency& dep_primary
						,ResourceObject::Reader rc_reader)=0;
				};


			virtual void targetsCreate(TagExtractor& extractor,const char* name_src
				,const char* in_dir,DependencyCollector& collector
				,Spider& spider
				,DependencyGraph& graph)=0;

			virtual void targetsCreate(TagExtractor& extractor,const char* in_dir
				,DependencyCollector& cb,Spider& spider,DependencyGraph& graph)=0;


			virtual size_t idGet() noexcept=0;

			virtual const char* rootGet() const noexcept=0;

			virtual void targetsLoad(const char* filename,const char* in_dir,Spider& spider
				,DependencyGraph& targets)=0;


			virtual Target_FactoryDelegator& factoryRegister(const Stringkey& filename_ext
				,const Target_Factory& factory)=0;

			virtual void factoriesUnregister()=0;

			virtual Target_FactoryDelegator& loaderRegister(const Stringkey& filename_ext
				,const Target_Loader& loader)=0;

			virtual void loadersUnregister() noexcept=0;

		protected:
			~Target_FactoryDelegator()=default;
		};
	}

#endif
