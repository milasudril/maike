//@	{
//@	 "targets":[{"name":"target_factorydefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_factorydefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_FACTORYDEFAULT_HPP
#define MAIKE_TARGET_FACTORYDEFAULT_HPP

#include "target_factory.hpp"
#include "targetcxxcompiler.hpp"
#include "targetcxxoptions.hpp"
#include "targetpythoninterpreter.hpp"

namespace Maike
	{
	class Target_FactoryDefault:public Target_Factory
		{
		public:
			Target_FactoryDefault(const ResourceObject& config,const ParameterSet& sysvars);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t id);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir
				,size_t id);


			void targetsCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t id,Callback&& cb);

			void targetsCreate(const ResourceObject& obj,const char* in_dir
				,size_t id,Callback&& cb);

			const TargetCxxOptions& cxxoptionsGet() const noexcept
				{return m_cxxoptions;}


		private:
			TargetCxxOptions m_cxxoptions;
			TargetCxxCompiler m_cxxcompiler;
			TargetPythonInterpreter m_pyintpret;
		};
	}

#endif
