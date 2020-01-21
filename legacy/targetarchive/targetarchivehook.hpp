//@	{
//@	 "targets":[{"name":"targetarchivehook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchivehook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETARCHIVEHOOK_HPP
#define MAIKE_TARGETARCHIVEHOOK_HPP

#include "../target_hook.hpp"

#include "targetarchivecompiler.hpp"
#include "targetarchiveloader.hpp"

namespace Maike
	{
	class PRIVATE TargetArchiveHook final:public Target_Hook
		{
		public:
			static TargetArchiveHook* create(const ParameterSetDumpable& params);

			static TargetArchiveHook* create(ParameterSet&& params)=delete;

			const TargetArchiveLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetArchiveHook& configAppendDefault();

			TargetArchiveHook& configAppend(const ResourceObject& archiveoptions);

			void configDump(ResourceObject& archiveoptions) const;

		private:
			TargetArchiveCompiler m_intpret;
			TargetArchiveLoader m_loader;

			explicit TargetArchiveHook(const ParameterSetDumpable& params);

			void destroy() noexcept;
		};
	}

#endif
