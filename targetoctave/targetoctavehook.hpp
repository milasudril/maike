//@	{
//@	 "targets":[{"name":"targetoctavehook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctavehook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETOCTAVEHOOK_HPP
#define MAIKE_TARGETOCTAVEHOOK_HPP

#include "../target_hook.hpp"

#include "targetoctaveinterpreter.hpp"
#include "targetoctaveloader.hpp"
#include "targetoctavefactory.hpp"

namespace Maike
	{
	class PRIVATE TargetOctaveHook final:public Target_Hook
		{
		public:
			static TargetOctaveHook* create(const ParameterSetDumpable& params);

			static TargetOctaveHook* create(ParameterSet&& params)=delete;


			const TargetOctaveFactory& factoryGet() const noexcept
				{return m_factory;}

			const TargetOctaveLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetOctaveHook& configAppendDefault();

			TargetOctaveHook& configAppend(const ResourceObject& octaveoptions);

			void configDump(ResourceObject& octaveoptions) const;

		private:
			TargetOctaveInterpreter m_intpret;
			TargetOctaveLoader m_loader;
			TargetOctaveFactory m_factory;

			explicit TargetOctaveHook(const ParameterSetDumpable& params);

			void destroy() noexcept;
		};
	}

#endif
