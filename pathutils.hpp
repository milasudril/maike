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
		if(*a=='\0')
			{return directoryNormalize(b);}
		std::string ret(a);
		ret+='/';
		ret+=b;
		return directoryNormalize(ret.c_str());
		}

	PRIVATE inline std::string dircat(const char* a,const std::string& b)
		{return dircat(a,b.c_str());}

	PRIVATE inline std::string dircat(const std::string& a,const char* b)
		{return dircat(a.c_str(),b);}

	PRIVATE inline std::string dircat(const std::string& a,const std::string& b)
		{return dircat(a.c_str(),b.c_str());}

	PRIVATE std::string rootStrip(const char* path,const char* root);

	PRIVATE inline std::string rootStrip(const std::string& path,const char* root)
		{return rootStrip(path.c_str(),root);}

	PRIVATE std::string exename();

	PRIVATE std::string getcwd();

	PRIVATE std::string homedir();

	PRIVATE std::string configdir();

	PRIVATE std::string configdirUser();
	}

#endif
