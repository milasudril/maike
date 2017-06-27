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

			const TargetCPLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetCPHook& configAppendDefault();

			TargetCPHook& configAppend(const ResourceObject& pythonoptions);

			void configDump(ResourceObject& pythonoptions) const;

		private:
			TargetCPLoader m_loader;

			void destroy() noexcept;
		};
	}

#endif
