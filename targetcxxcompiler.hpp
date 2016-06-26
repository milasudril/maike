//@	{
//@	 "targets":[{"name":"targetcxxcompiler.hpp","type":"include"}]
//@	,"dependencies_extra:"[{"ref":targetcxxcompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXCOMPILER_HPP
#define MAIKE_TARGETCXXCOMPILER_HPP

#include "targetcompiler.hpp"
#include "targetcxxoptions.hpp"
#include "twins.hpp"
#include <string>

namespace Maike
	{
	class Dependency;
	class Target;

	class TargetCxxCompiler:public TargetCompiler
		{
		public:
			TargetCxxCompiler& directoryTargetsSet(const char* dirname);

			void compile(const char* source,const char* dest
				,const TargetCxxOptions& options) const;

			void link(const Twins<const char* const>& files
				,const char* dest
				,const TargetCxxOptions& options) const;

		private:
			std::string m_dir_targets;
			TargetCxxOptions m_options;
		};
	}

#endif

