//@	{
//@	 "targets":[{"name":"local_execp.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"local_execp.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_LOCALEXECP_HPP
#define MAIKE_LOCALEXECP_HPP

#include "./fs.hpp"
#include "./exec_result.hpp"
#include "./io_redirector.hpp"

#include <cstddef>

namespace Maike
{
	ExecResult execp(fs::path const& executable,
	                 std::vector<std::string> const& args,
	                 IoRedirector const& io_redir);

	template<class IoRedirectorType>
	ExecResult execp(fs::path const& executable,
	                 std::vector<std::string> const& args,
	                 IoRedirectorType&& io_redir,
	                 std::enable_if_t<!std::is_same_v<std::decay_t<IoRedirectorType>, IoRedirector>, int> = 0)
	{
		return execp(executable, args, IoRedirector{io_redir});
	}
}

#endif
