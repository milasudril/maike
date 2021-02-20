//@	{
//@	 "targets":[{"name":"targetcphook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcphook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETXMLHOOK_HPP
#define MAIKE_TARGETXMLHOOK_HPP

#include "../target_hook.hpp"

#include "targetcploader.hpp"

namespace Maike
	{
	class PRIVATE TargetCPHook final:public Target_Hook
		{
		public:
			static TargetCPHook* create();

			const TargetCPLoader& loaderGet() const noexcept override
				{return m_loader;}

			void configClear() override;

			TargetCPHook& configAppendDefault() override;

			TargetCPHook& configAppend(const ResourceObject& options) override;

			void configDump(ResourceObject& options) const override;

		private:
			TargetCPLoader m_loader;

			void destroy() noexcept override;
		};
	}

#endif
