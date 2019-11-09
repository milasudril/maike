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

			const TargetTrivialLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetTrivialHook& configAppendDefault();

			TargetTrivialHook& configAppend(const ResourceObject& opts);

			void configDump(ResourceObject& opts) const;

		private:
			TargetTrivialOptions m_opts;
			TargetTrivialLoader m_loader;

			void destroy() noexcept;
		};
	}

#endif
