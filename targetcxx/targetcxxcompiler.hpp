//@	{
//@	 "targets":[{"name":"targetcxxcompiler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxcompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXCOMPILER_HPP
#define MAIKE_TARGETCXXCOMPILER_HPP

#include "targetcxxpkgconfig.hpp"
#include "../twins.hpp"
#include "../visibility.hpp"
#include "../stringkey.hpp"
#include <string>
#include <vector>
#include <map>

namespace Maike
	{
	class Dependency;
	class Target;
	class TargetCxxOptions;
	class ParameterSet;
	class Command;

	class PRIVATE TargetCxxCompiler
		{
		public:
			explicit TargetCxxCompiler(const TargetCxxOptions& options,const ParameterSet& sysvars);

			TargetCxxCompiler(const TargetCxxOptions& options,ParameterSet&& params)=delete;

			TargetCxxCompiler(TargetCxxOptions&& options,ParameterSet&& params)=delete;

			TargetCxxCompiler(const TargetCxxOptions&& options,const ParameterSet& params)=delete;

			void compileObject(const char* source,const char* in_dir,const char* dest
				,const TargetCxxOptions& options_extra) const;

			enum class FileUsage:unsigned int{NORMAL,LIB_INTERNAL,LIB_EXTERNAL};
			struct FileInfo
				{
				const char* filename;
				FileUsage usage;
				};
			void compileApplication(const char* source,const char* in_dir,Twins<const FileInfo*> dependencies
				,const char* dest,const TargetCxxOptions& options_extra) const;

			void compileDll(const char* source,const char* in_dir,Twins<const FileInfo*> dependencies
				,const char* dest,const TargetCxxOptions& options_extra) const;

			void compileLibrary(const char* source,const char* in_dir,Twins<const FileInfo*> dependencies
				,const char* dest,const TargetCxxOptions& options_extra) const;

			const TargetCxxOptions& optionsGet() const noexcept
				{return r_options;}

			unsigned long long int cxxversionDefaultGet() const;

			const PkgConfigRequest& pkgconfigAsk(const char* library,const char* context) const;

		private:
			const TargetCxxOptions& r_options;
			const ParameterSet* r_paramset;
			mutable unsigned long long int m_cxxversion_default;
			mutable std::map<Stringkey,PkgConfigRequest> m_pkgconfig_cache;

			void execute(const Command& cmd,const char* source
				,const char* in_dir
				,Twins<const FileInfo*> dependencies
				,const char* dest,const TargetCxxOptions& options_extra) const;
		};
	}

#endif

