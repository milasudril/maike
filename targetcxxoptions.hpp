//@	{
//@	 "targets":[{"name":"targetcxxoptions.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxoptions.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXOPTIONS_HPP
#define MAIKE_TARGETCXXOPTIONS_HPP


#include "command.hpp"
#include "parametersetdefault.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	class ResourceObject;

	class TargetCxxOptions
		{
		public:
			TargetCxxOptions(const ResourceObject& options
				,ParameterSet&& paramset_global)=delete;

			TargetCxxOptions(const ResourceObject& options
				,const ParameterSet& paramset_global);

		private:
			std::vector<const ParameterSet*> r_paramset;
			ParameterSetDefault m_paramset;

			std::vector< std::string > m_includedir;
			std::vector< std::string > m_libdir;
			std::string m_platform_suffix;

			std::string m_includedir_format;
			std::string m_libdir_format;
			std::string m_libext_format;
			std::string m_libint_format;

			long long int m_cxxversion_min;
			long long int m_cxxversion_default;
			std::string m_stdprefix;

			Command m_appcompile;
			Command m_dllcompile;
			Command m_libcompile;
			Command m_objcompile;
			Command m_versionquery;

			long long int cxxversionDefaultGet() const;
		};
	}

#endif
