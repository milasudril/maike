//@	{
//@	  "targets":[{"name":"local_system_invoker.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"local_system_invoker.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_EXEC_LOCALSYSTEMINVOKER_HPP
#define MAIKE_EXEC_LOCALSYSTEMINVOKER_HPP

#include "./local_execp.hpp"

#include "src/fs.hpp"

#include <sys/stat.h>
#include <sys/types.h>

namespace Maike::Exec
{
	class LocalSystemInvoker
	{
	public:
		void mkdir(fs::path const& dir) const
		{
			if(::mkdir(dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == -1)
			{ throw std::runtime_error{"Failed to create directory"}; }
		}

		void saveBuffer(std::byte const* buffer, size_t n, fs::path const& file) const;

		void copyFile(fs::path const& src, fs::path const& dest) const
		{
			std::error_code ec;
			if(!copy_file(src, dest, ec)) { throw std::runtime_error{"Failed to copy file"}; }
		}

		decltype(auto) execp(fs::path const& executable,
		                     std::vector<std::string> const& args,
		                     Io::Redirector const& redir) const
		{
			return Maike::Exec::execp(executable, args, redir);
		}

	private:
	};

	template<class IoRedirectorType>
	decltype(auto) execp(LocalSystemInvoker const& invoker,
	                     fs::path const& exe,
	                     std::vector<std::string> const& args,
	                     IoRedirectorType&& redir)
	{
		return invoker.execp(exe, args, Io::Redirector{redir});
	}
}

#endif
