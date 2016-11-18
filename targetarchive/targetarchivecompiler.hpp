//@	{
//@  "targets":[{"name":"targetarchivecompiler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchivecompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETARCHIVECOMPILER_HPP
#define MAIKE_TARGETARCHIVECOMPILER_HPP

#include "taroptions.hpp"

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

			int tar(Twins<const char* const*> files,const char* name
				,const char* target_dir,const char* root
				,const char* compressor) const;

			int zip(Twins<const char* const*> files,const char* name
				,const char* target_dir,const char* root) const;

			void configClear();

			TargetArchiveCompiler& configAppendDefault();

			TargetArchiveCompiler& configAppend(const ResourceObject& archiveoptions);

			void configDump(ResourceObject& archiveoptions) const;

		private:
			Taroptions m_tar;
			const ParameterSetDumpable& r_sysvars;
		};
	}

#endif
