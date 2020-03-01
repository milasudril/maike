//@	{
//@	"targets":[{"name":"spider.hpp","type":"include"}]
//@	}

#ifndef MAIKE_SPIDER_HPP
#define MAIKE_SPIDER_HPP

#include "visibility.hpp"
#include <memory>

namespace Maike
{
	class Target_Loader;
	class Stringkey;

	class PRIVATE Spider
	{
	public:
		virtual Spider& scanFile(const char* filename, const char* in_dir) = 0;
		virtual Spider& run() = 0;

	protected:
		~Spider() = default;
	};
}

#endif
