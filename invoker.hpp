//@	{
//@	 "targets":[{"name":"invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_INVOKER_HPP
#define MAIKE_INVOKER_HPP

#include "./fs.hpp"

namespace Maike
{
	class Invoker
	{
		public:
			int mkdir(fs::path const& dir);
			int writeToFile(std::string const& str, fs::path const& file);
			int copy(fs::path const& src, fs::path const& dest);

			CmdResult execp(Command const& cmd);

		private:
	};
}

#endif