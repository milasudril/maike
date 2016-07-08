//@	{
//@	 "targets":[{"name":"target_factorydelegatordefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_factorydelegatordefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_FACTORYDELEGATORDEFAULT_HPP
#define MAIKE_TARGET_FACTORYDELEGATORDEFAULT_HPP

#include "target_factorydelegator.hpp"
#include <map>
#include <string>

namespace Maike
	{
	class ExpressionEvaluator;
	class Target_FactoryDelegatorDefault:public Target_FactoryDelegator
		{
		public:
			explicit Target_FactoryDelegatorDefault(const char* target_dir
				,const ExpressionEvaluator& eval);

			Target_FactoryDelegatorDefault(ExpressionEvaluator&& eval)=delete;

			Target_FactoryDelegatorDefault& factoryRegister(const Stringkey& filename_ext
				,const Target_Factory& factory);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir);

			void targetsCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,Callback&& cb);

			void targetsCreate(const ResourceObject& obj,const char* in_dir
				,Callback&& cb);

			size_t idNext() noexcept
				{
				auto ret=m_id_current;
				++m_id_current;
				return ret;
				}

		private:
			void targetsCreateImpl(const ResourceObject& obj,const char* name_src
				,const char* in_dir,Callback& cb);

			const ExpressionEvaluator& r_eval;
			std::map<Stringkey,const Target_Factory*> m_r_factories;
			std::string m_target_dir;
			size_t m_id_current;
		};
	}

#endif