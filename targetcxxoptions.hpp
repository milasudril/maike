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
			TargetCxxOptions();

			TargetCxxOptions(const ResourceObject& options);

			const Command& versionqueryGet() const noexcept
				{return m_versionquery;}

			const Command& objcompileGet() const noexcept
				{return m_objcompile;}

			const Command& appcompileGet() const noexcept
				{return m_appcompile;}

			const Command& dllcompileGet() const noexcept
				{return m_dllcompile;}

			const ParameterSetDefault& paramsGet() const noexcept
				{return m_params;}

			long long int cxxversionMinGet() const noexcept
				{return m_cxxversion_min;}

			const char* stdprefixGet() const noexcept
				{return m_stdprefix.c_str();}

			const char* libextFormatGet() const noexcept
				{return m_libext_format.c_str();}

			const char* libintFormatGet() const noexcept
				{return m_libext_format.c_str();}

		private:
			uint64_t m_fields_valid;
			ParameterSetDefault m_params;

			std::vector< std::string > m_includedir;
			std::vector< std::string > m_libdir;

			std::string m_platform_suffix;
			static constexpr uint64_t PLATFORMSUFFIX_VALID=0x1;

			std::string m_includedir_format;
			std::string m_libdir_format;
			std::string m_libext_format;
			std::string m_libint_format;

			long long int m_cxxversion_min;
			static constexpr uint64_t CXXVERSIONMIN_VALID=0x2;

			std::string m_stdprefix;
			static constexpr uint64_t STDPREFIX_VALID=0x8;

			Command m_appcompile;
			Command m_dllcompile;
			Command m_libcompile;
			Command m_objcompile;
			Command m_versionquery;
		};
	}

#endif
