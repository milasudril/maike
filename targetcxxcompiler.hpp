//@	{
//@	 "targets":[{"name":"targetcxxcompiler.hpp","type":"include"}]
//@	,"dependencies_extra:"[{"ref":targetcxxcompiler.o","rel":"implementation"}]
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
			void compile(const char* source,const char* dest
				,Invoker& invoker,const TargetCxxOptions& options) const;

			void link(const Twins<const char* const>& files
				,const char* dest,Invoker& invoker
				,const TargetCxxOptions& options) const;

		private:
			TargetCxxOptions m_options;
		};
	}

#endif

