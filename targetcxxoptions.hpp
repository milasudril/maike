//@	{"targets":[{"name":"targetcxxoptions.hpp","type":"include"}]}

#ifndef MAIKE_TARGETCXXOPTIONS_HPP
#define MAIKE_TARGETCXXOPTIONS_HPP

namespace Maike
	{
	class TargetCxxOptions
		{
		public:

		private:
			std::vector< std::string > m_include_dir;
			std::vector< std::string > m_lib_dir;
		};
	}

#endif
