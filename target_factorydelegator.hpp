//@	{"targets":[{"name":"target_factorydelegator.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_FACTORYDELEGATOR_HPP
#define MAIKE_TARGET_FACTORYDELEGATOR_HPP

#include "handle.hpp"
#include "visibility.hpp"
#include "datasource.hpp"

namespace Maike
	{
	class Target;
	class Target_Factory;
	class Stringkey;
	class ResourceObject;
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

			class Callback
				{
				public:
					virtual void operator()(const Target_FactoryDelegator&,Handle<Target>&& target)=0;
				};

			class DependencyCollector
				{
				public:
					virtual void operator()(const Target_FactoryDelegator&,ResourceObject& dep_array)=0;
				};

			virtual void targetsCreate(TagExtractor&& extractor,const char* name_src
				,const char* in_dir,Callback&& cb)=0;

			virtual void targetsCreate(TagExtractor&& extractor,const char* in_dir
				,Callback&& cb)=0;

			virtual Target_FactoryDelegator& factoryRegister(const Stringkey& filename_ext
				,const Target_Factory& factory)=0;

			virtual void factoriesUnregister()=0;

			virtual size_t idGet() noexcept=0;

			virtual const char* rootGet() const noexcept=0;

		protected:
			~Target_FactoryDelegator()=default;
		};
	}

#endif
