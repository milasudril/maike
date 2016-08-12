//@	{
//@	 "targets":[{"name":"pathutils.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"pathutils.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PATHUTILS_HPP
#define MAIKE_PATHUTILS_HPP
#include "visibility.hpp"
#include <string>

namespace Maike
	{
	PRIVATE std::string directoryNormalize(const char* str);

	PRIVATE inline std::string dirname(const std::string& path)
		{
		if(path==".")
			{return "";}
		auto pos=path.find_last_of('/');
		if(pos==std::string::npos)
			{return std::string(".");}
		return directoryNormalize(path.substr(0,pos).c_str());
		}

	PRIVATE inline std::string dircat(const char* a,const char* b)
		{
		std::string ret(a);
		ret+='/';
		ret+=b;
		return directoryNormalize(ret.c_str());
		}

	PRIVATE inline std::string dircat(const char* a,const std::string& b)
		{
		std::string ret(a);
		ret+='/';
		ret+=b;
		return directoryNormalize(ret.c_str());
		}

	PRIVATE inline std::string dircat(const std::string& a,const char* b)
		{
		std::string ret(a);
		ret+='/';
		ret+=b;
		return directoryNormalize(ret.c_str());
		}

	PRIVATE std::string rootStrip(const std::string& path,const char* root);
	}

#endif
