//@	{"targets":[{"name":"targetcxxoptions.hpp","type":"include"}]}

#ifndef MAIKE_TARGETCXXOPTIONS_HPP
#define MAIKE_TARGETCXXOPTIONS_HPP


#include "command.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	class TargetCxxOptions
		{
		public:

		private:
			std::vector< std::string > m_include_dir;
			std::vector< std::string > m_lib_dir;
			std::string m_platform_prefix;
			std::string m_includedir_format;
			std::string m_libdir_format;
			std::string m_libext_format;
			std::string m_libint_format;

			Command m_app_cmd;
			Command m_lib_cmd;
			Command m_dll_cmd;
			Command m_object_cmd;
		};
	}

#endif
