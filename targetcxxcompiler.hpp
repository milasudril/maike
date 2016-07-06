//@	{
//@	 "targets":[{"name":"targetcxxcompiler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxcompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXCOMPILER_HPP
#define MAIKE_TARGETCXXCOMPILER_HPP

#include "twins.hpp"
#include <string>
#include <vector>

namespace Maike
	{
	class Dependency;
	class Target;
	class TargetCxxOptions;
	class ParameterSet;

	class TargetCxxCompiler
		{
		public:
			explicit TargetCxxCompiler(const TargetCxxOptions& options,const ParameterSet& sysvars);

			TargetCxxCompiler(const TargetCxxOptions& options,ParameterSet&& params)=delete;

			void compileObject(const char* source,const char* dest
				,const TargetCxxOptions& options_extra) const;

			enum class FileUsage:unsigned int{NORMAL,LIB_INTERNAL,LIB_EXTERNAL};
			struct FileInfo
				{
				const char* filename;
				FileUsage usage;
				};
			void compileApplication(Twins<const FileInfo*> files
				,const char* dest,const TargetCxxOptions& options_extra) const;

			void compileDll(Twins<const FileInfo*> files
				,const char* dest,const TargetCxxOptions& options_extra) const;

			void compileLibrary(Twins<const FileInfo*> files
				,const char* dest,const TargetCxxOptions& options_extra) const;

			const TargetCxxOptions& optionsGet() const noexcept
				{return r_options;}

		private:
			const TargetCxxOptions& r_options;
			std::vector<const ParameterSet*> r_paramset;
			long int m_cxxversion_default;
		};
	}

#endif

