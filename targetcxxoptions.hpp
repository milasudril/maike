//@	{
//@	 "targets":[{"name":"targetcxxoptions.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxoptions.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXOPTIONS_HPP
#define MAIKE_TARGETCXXOPTIONS_HPP


#include "command.hpp"
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

			long long int cxxversionMinGet() const noexcept
				{return m_cxxversion_min;}

			long long int cxxversionMaxGet() const noexcept
				{return m_cxxversion_max;}

			const char* stdprefixGet() const noexcept
				{return m_stdprefix.c_str();}

			const char* libextFormatGet() const noexcept
				{return m_libext_format.c_str();}

			const char* libintFormatGet() const noexcept
				{return m_libext_format.c_str();}

			void configClear();
			TargetCxxOptions& configAppendDefault();
			TargetCxxOptions& configAppend(const ResourceObject& cxxoptions);

			void configDump(ResourceObject& cxxoptions) const;


		private:
			std::vector< std::string > m_includedir;
			std::vector< std::string > m_libdir;

			std::string m_platform_suffix;

			std::string m_includedir_format;
			std::string m_libdir_format;
			std::string m_libext_format;
			std::string m_libint_format;

			long long int m_cxxversion_min;
			long long int m_cxxversion_max;

			std::string m_stdprefix;

			Command m_appcompile;
			Command m_dllcompile;
			Command m_libcompile;
			Command m_objcompile;
			Command m_versionquery;
		};
	}

#endif
