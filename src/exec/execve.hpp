//@	{
//@	 "targets":[{"name":"execve.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"./execve.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_EXEC_EXECVE_HPP
#define MAIKE_EXEC_EXECVE_HPP

#include "./result.hpp"
#include "./command.hpp"

#include "src/utils/fs.hpp"
#include "src/io/io_redirector.hpp"

#include <cstddef>
#include <vector>

namespace Maike::Exec
{
	ExitStatus execve(fs::path const& executable,
	                  std::vector<std::string> const& args,
	                  Io::Redirector const& io_redir);

	template<class IoRedirectorType>
	ExitStatus
	execve(fs::path const& executable,
	       std::vector<std::string> const& args,
	       IoRedirectorType&& io_redir,
	       std::enable_if_t<!std::is_same_v<std::decay_t<IoRedirectorType>, Io::Redirector>, int> = 0)
	{
		return execve(executable, args, Io::Redirector{io_redir});
	}

	Result execve(fs::path const& executable, std::vector<std::string> const& args);

	struct LocalExecve
	{
		bool dryrun;
	};

	Result execve(LocalExecve cfg, Command const& cmd);
}

#endif
