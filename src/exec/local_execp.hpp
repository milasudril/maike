//@	{
//@	 "targets":[{"name":"local_execp.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"local_execp.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_EXEC_LOCALEXECP_HPP
#define MAIKE_EXEC_LOCALEXECP_HPP

#include "./result.hpp"

#include "src/fs.hpp"
#include "src/io/io_redirector.hpp"

#include <cstddef>
#include <vector>

namespace Maike::Exec
{
	Result execp(fs::path const& executable,
	             std::vector<std::string> const& args,
	             IoRedirector const& io_redir);

	template<class IoRedirectorType>
	Result
	execp(fs::path const& executable,
	      std::vector<std::string> const& args,
	      IoRedirectorType&& io_redir,
	      std::enable_if_t<!std::is_same_v<std::decay_t<IoRedirectorType>, IoRedirector>, int> = 0)
	{
		return execp(executable, args, IoRedirector{io_redir});
	}
}

#endif
