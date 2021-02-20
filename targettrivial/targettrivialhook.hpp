//@	{
//@	 "targets":[{"name":"targettrivialhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targettrivialhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETTRIVIALHOOK_HPP
#define MAIKE_TARGETTRIVIALHOOK_HPP

#include "../target_hook.hpp"

#include "targettrivialloader.hpp"
#include "targettrivialoptions.hpp"

namespace Maike
	{
	class PRIVATE TargetTrivialHook final:public Target_Hook
		{
		public:
			static TargetTrivialHook* create();

			const TargetTrivialLoader& loaderGet() const noexcept override
				{return m_loader;}

			void configClear() override;

			TargetTrivialHook& configAppendDefault() override;

			TargetTrivialHook& configAppend(const ResourceObject& opts) override;

			void configDump(ResourceObject& opts) const override;

		private:
			TargetTrivialOptions m_opts;
			TargetTrivialLoader m_loader;

			void destroy() noexcept override;

			TargetTrivialHook();
		};
	}

#endif
