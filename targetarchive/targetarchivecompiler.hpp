//@	{
//@  "targets":[{"name":"targetarchivecompiler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchivecompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETARCHIVECOMPILER_HPP
#define MAIKE_TARGETARCHIVECOMPILER_HPP

#include "../command.hpp"

namespace Maike
	{
	class ResourceObject;
	class ParameterSetDumpable;
	class PRIVATE TargetArchiveCompiler
		{
		public:
			explicit TargetArchiveCompiler(const ParameterSetDumpable& sysvars);

			explicit TargetArchiveCompiler(ParameterSetDumpable&& sysvars)=delete;

			TargetArchiveCompiler(const ResourceObject& archiveoptions
				,ParameterSet&& sysvars)=delete;

			int run(const char* script,Twins<const char* const*> args) const;

			void configClear();

			TargetArchiveCompiler& configAppendDefault();

			TargetArchiveCompiler& configAppend(const ResourceObject& archiveoptions);

			void configDump(ResourceObject& archiveoptions) const;

		private:
			Command m_compiler;
			const ParameterSetDumpable& r_sysvars;
		};
	}

#endif
