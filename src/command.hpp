//@	{"targets":[{"name":"command.hpp","type":"include"}]}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "./fs.hpp"

#include <string>
#include <vector>

namespace Maike
{
	class Command
	{
	public:
	private:
		fs::path m_executable;
		std::vector<std::string> m_args;
	};
}

#endif
