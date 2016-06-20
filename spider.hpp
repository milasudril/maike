//@{
//@"targets":[{"name":"spider.hpp","type":"include"}]
//@}

#ifndef MAIKE_SPIDER_HPP
#define MAIKE_SPIDER_HPP

#include <memory>

namespace Maike
	{
	class Target;

	class Spider
		{
		public:
			virtual Spider& scan(const char* directory)=0;
			virtual Spider& filenameScan(const char* filename)=0;
			virtual Spider& targetRegister(std::unique_ptr<Target>&& target)=0;
			virtual const char* targetPrefixGet() const noexcept=0;
			virtual const char* sourcePrefixGet() const noexcept=0;
		};
	}

#endif
