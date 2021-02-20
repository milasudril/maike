//@	{
//@	 "targets":[{"name":"targetxmlhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetxmlhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETXMLHOOK_HPP
#define MAIKE_TARGETXMLHOOK_HPP

#include "../target_hook.hpp"

#include "targetxmlloader.hpp"

namespace Maike
	{
	class PRIVATE TargetXMLHook final:public Target_Hook
		{
		public:
			static TargetXMLHook* create();

			const TargetXMLLoader& loaderGet() const noexcept override
				{return m_loader;}

			void configClear() override;

			TargetXMLHook& configAppendDefault() override;

			TargetXMLHook& configAppend(const ResourceObject& pythonoptions) override;

			void configDump(ResourceObject& pythonoptions) const override;

		private:
			TargetXMLLoader m_loader;

			void destroy() noexcept override;
		};
	}

#endif
