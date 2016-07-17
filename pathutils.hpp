//@	{"targets":[{"name":"pathutils.hpp","type":"include"}]}

#ifndef MAIKE_PATHUTILS_HPP
#define MAIKE_PATHUTILS_HPP

#include <string>

namespace Maike
	{
	inline std::string dirname(const std::string& path)
		{
		auto pos=path.find_last_of('/');
		if(pos==std::string::npos)
			{return std::string(".");}
		return path.substr(0,pos);
		}

	inline std::string dircat(const char* a,const char* b)
		{
		std::string ret(a);
		ret+='/';
		ret+=b;
		return std::move(ret);
		}

	inline std::string dircat(const char* a,const std::string& b)
		{
		std::string ret(a);
		ret+='/';
		ret+=b;
		return std::move(ret);
		}

	inline std::string dircat(const std::string& a,const char* b)
		{
		std::string ret(a);
		ret+='/';
		ret+=b;
		return std::move(ret);
		}
	}

#endif
