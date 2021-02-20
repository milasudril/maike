//@	{
//@	 "targets":[{"name":"targetbashhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetbashhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETBASHHOOK_HPP
#define MAIKE_TARGETBASHHOOK_HPP

#include "../target_hook.hpp"

#include "targetbashinterpreter.hpp"
#include "targetbashloader.hpp"

namespace Maike
	{
	class PRIVATE TargetBashHook final:public Target_Hook
		{
		public:
			static TargetBashHook* create(const ParameterSetDumpable& params);

			static TargetBashHook* create(ParameterSet&& params)=delete;

			const TargetBashLoader& loaderGet() const noexcept override
				{return m_loader;}

			void configClear() override;

			TargetBashHook& configAppendDefault() override;

			TargetBashHook& configAppend(const ResourceObject& options) override;

			void configDump(ResourceObject& options) const override;

		private:
			TargetBashInterpreter m_intpret;
			TargetBashLoader m_loader;

			explicit TargetBashHook(const ParameterSetDumpable& params);

			void destroy() noexcept override;
		};
	}

#endif
