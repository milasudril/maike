//@	{
//@	 "targets":[{"name":"targetcxxcompiler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxcompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXCOMPILER_HPP
#define MAIKE_TARGETCXXCOMPILER_HPP

#include "targetcxxoptions.hpp"
#include "twins.hpp"
#include <string>

namespace Maike
	{
	class Dependency;
	class Target;

	class TargetCxxCompiler
		{
		public:
			TargetCxxCompiler(const TargetCxxOptions& options);

			TargetCxxCompiler(ResourceObject&& options)=delete;

			void compileObject(const char* source,const char* dest
				,const TargetCxxOptions& options_extra) const;

			void compileApplication(const Twins<const char* const>& files
				,const char* dest,const TargetCxxOptions& options_extra) const;

			void compileDll(const Twins<const char* const>& files
				,const char* dest,const TargetCxxOptions& options_extra) const;

			void compileLibrary(const Twins<const char* const>& files
				,const char* dest,const TargetCxxOptions& options_extra) const;

		private:
			const TargetCxxOptions& r_options;
			long long int m_cxxversion_default;
		};
	}

#endif

