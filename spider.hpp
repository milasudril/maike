//@	{
//@	"targets":[{"name":"spider.hpp","type":"include"}]
//@	}

#ifndef MAIKE_SPIDER_HPP
#define MAIKE_SPIDER_HPP

#include <memory>

namespace Maike
	{
	class Target;

	class Spider
		{
		public:
			virtual Spider& scanFile(const char* filename,const char* in_dir)=0;
			virtual Spider& run()=0;
		};
	}

#endif
