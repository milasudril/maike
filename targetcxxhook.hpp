//@	{
//@	 "targets":[{"name":"targetcxxhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXHOOK_HPP
#define MAIKE_TARGETCXXHOOK_HPP

#include "target_hook.hpp"

#include "targetcxxcompiler.hpp"
#include "targetcxxoptions.hpp"
#include "targetcxxloader.hpp"
#include "targetcxxfactory.hpp"

namespace Maike
	{
	class TargetCxxHook final:public Target_Hook
		{
		public:
			static TargetCxxHook* create(const ResourceObject& cxxoptions
				,const ParameterSet& params);


			const TargetCxxFactory& factoryGet() const noexcept
				{return m_factory;}

			const TargetCxxLoader& loaderGet() const noexcept
				{return m_loader;}

		private:
			TargetCxxOptions m_options;
			TargetCxxCompiler m_compiler;
			TargetCxxLoader m_loader;
			TargetCxxFactory m_factory;

			explicit TargetCxxHook(const ResourceObject& cxxoptions
				,const ParameterSet& params);

			explicit TargetCxxHook(const ResourceObject& cxxoptions
				,ParameterSet&& params)=delete;

			void destroy() noexcept;
		};
	}

#endif
