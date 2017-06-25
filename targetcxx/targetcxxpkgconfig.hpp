//@	{
//@	"targets":[{"name":"targetcxxpkgconfig.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxpkgconfig.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXPKGCONFIG_HPP
#define MAIKE_TARGETCXXPKGCONFIG_HPP

#include "../visibility.hpp"
#include "../dependency.hpp"

#include <vector>
#include <string>

namespace Maike
	{
	class Command;
	class Target;
	class TargetCxxOptions;
	class Dependency;

	class PRIVATE PkgConfigRequest
		{
		public:
			PkgConfigRequest(const Command& cmd,const char* libname
				,const char* context);

			const PkgConfigRequest& optionsPush(TargetCxxOptions& options) const;
			const PkgConfigRequest& dependenciesPush(Target& t) const;

		private:
			std::vector<std::string> m_incdir;
			std::vector<std::string> m_cflags;
			std::vector<std::string> m_libdir;
			std::vector<Dependency> m_deps;
		};
	}

#endif
