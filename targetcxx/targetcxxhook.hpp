//@	{
//@	 "targets":[{"name":"targetcxxhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXHOOK_HPP
#define MAIKE_TARGETCXXHOOK_HPP

#include "../target_hook.hpp"

#include "targetcxxcompiler.hpp"
#include "targetcxxoptions.hpp"
#include "targetcxxloader.hpp"

#include "../parametersetdumpable.hpp"

namespace Maike
	{
	class PRIVATE TargetCxxHook final:public Target_Hook
		{
		public:
			static TargetCxxHook* create(const ParameterSet& params);

			static TargetCxxHook* create(ParameterSet&& params)=delete;

			const TargetCxxLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();
			TargetCxxHook& configAppendDefault();
			TargetCxxHook& configAppend(const ResourceObject& cxxoptions);
			void configDump(ResourceObject& cxxoptions) const;

		private:
			TargetCxxOptions m_options;
			TargetCxxCompiler m_compiler;
			TargetCxxLoader m_loader;

			explicit TargetCxxHook(const ParameterSet& params);

			void destroy() noexcept;
		};
	}

#endif
