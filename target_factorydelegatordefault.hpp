//@	{
//@	 "targets":[{"name":"target_factorydelegatordefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_factorydelegatordefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_FACTORYDELEGATORDEFAULT_HPP
#define MAIKE_TARGET_FACTORYDELEGATORDEFAULT_HPP

#include "target_factorydelegator.hpp"
#include "idgenerator.hpp"
#include "visibility.hpp"
#include <map>

namespace Maike
	{
	class ExpressionEvaluator;
	class PRIVATE Target_FactoryDelegatorDefault:public Target_FactoryDelegator
		{
		public:
			explicit Target_FactoryDelegatorDefault(const ExpressionEvaluator& eval
				,IdGenerator<size_t>& id_gen);

			Target_FactoryDelegatorDefault(ExpressionEvaluator&& eval)=delete;

			Target_FactoryDelegatorDefault& factoryRegister(const Stringkey& filename_ext
				,const Target_Factory& factory);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t line_count);

			Handle<Target> targetCreate(const ResourceObject& obj,const char* in_dir,size_t line_count);

			void targetsCreate(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t line_count,Callback&& cb);

			void targetsCreate(const ResourceObject& obj,const char* in_dir
				,size_t line_count,Callback&& cb);

			size_t idGet() noexcept
				{return r_id_gen.idGet();}

		private:
			void targetsCreateImpl(const ResourceObject& obj,const char* name_src
				,const char* in_dir,size_t line_count,Callback& cb);

			const ExpressionEvaluator& r_eval;
			IdGenerator<size_t>& r_id_gen;
			std::map<Stringkey,const Target_Factory*> m_r_factories;
			size_t m_id_current;
		};
	}

#endif
