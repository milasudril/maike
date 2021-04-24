//@	{
//@	 "targets":[{"name":"command.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_EXEC_COMMAND_HPP
#define MAIKE_EXEC_COMMAND_HPP

#include "src/utils/fs.hpp"

namespace Maike::Exec
{
	struct Command
	{
		fs::path executable;
		std::vector<std::string> args;
	};
}

#endif