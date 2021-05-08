//@	{
//@	  "targets":[{"name":"command.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"./command.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_EXEC_COMMAND_HPP
#define MAIKE_EXEC_COMMAND_HPP

#include "src/utils/fs.hpp"

#include <vector>
#include <string>

namespace Maike::Exec
{
	struct Command
	{
		fs::path executable;
		std::vector<std::string> args;
	};

	std::string toString(Command const& cmd);
}

#endif