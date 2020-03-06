//@	{"targets":[{"name":"config.hpp","type":"include"}]}

#ifndef MAIKE_VCSINVOKER_CONFIG_HPP
#define MAIKE_VCSINVOKER_CONFIG_HPP

#include "src/command.hpp"

namespace Maike::VcsInvoker
{
	class Config
	{
	public:
	private:
		Command m_get_revision;
		Command m_get_version_tag;
		Command m_get_branch;
	};
}

#endif
