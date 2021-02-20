//@	{
//@	 "targets":[{"name":"targetoctavehook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctavehook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETOCTAVEHOOK_HPP
#define MAIKE_TARGETOCTAVEHOOK_HPP

#include "../target_hook.hpp"

#include "targetoctaveinterpreter.hpp"
#include "targetoctaveloader.hpp"

namespace Maike
	{
	class PRIVATE TargetOctaveHook final:public Target_Hook
		{
		public:
			static TargetOctaveHook* create(const ParameterSetDumpable& params);

			static TargetOctaveHook* create(ParameterSet&& params)=delete;

			const TargetOctaveLoader& loaderGet() const noexcept override
				{return m_loader;}

			void configClear() override;

			TargetOctaveHook& configAppendDefault() override;

			TargetOctaveHook& configAppend(const ResourceObject& octaveoptions) override;

			void configDump(ResourceObject& octaveoptions) const override;

		private:
			TargetOctaveInterpreter m_intpret;
			TargetOctaveLoader m_loader;

			explicit TargetOctaveHook(const ParameterSetDumpable& params);

			void destroy() noexcept override;
		};
	}

#endif
