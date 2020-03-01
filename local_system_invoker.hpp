//@	{
//@	 "targets":[{"name":"local_system_invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_LOCALSYSTEMINVOKER_HPP
#define MAIKE_LOCALSYSTEMINVOKER_HPP

#include "./fs.hpp"
#include "./local_execp.hpp"

#include <sys/stat.h>
#include <sys/types.h>

namespace Maike
{
	class LocalSystemInvoker
	{
	public:
		void mkdir(fs::path const& dir) const
		{
			if(mkdir(dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == -1)
			{ throw std::runtime_error{"Failed to create directory"}; }
		}

		int writeToFile(std::string const& str, fs::path const& file) const;

		void copy(fs::path const& src, fs::path const& dest) const
		{
			std::error_code sc;
			if(!copy_file(src, dest, ec)) { throw std::runtime_error{"Failed to copy file"}; }
		}

		template<class IoRedirector>
		decltype(auto) execp(Command const& cmd, IoRedirector&& redir) const
		{
			return cmd.execp(redir);
		}

	private:
	};
}

#endif