//@	{"targets":[{"name":"pathutils.hpp","type":"include"}]}

#ifndef MAIKE_PATHUTILS_HPP
#define MAIKE_PATHUTILS_HPP
#include "visibility.hpp"
#include <string>

namespace Maike
	{
	PRIVATE inline std::string directoryNormalize(const char* str)
		{
		std::string ret;
		std::string elem_current;
		while(*str!='\0')
			{
			switch(*str)
				{
				case '/':
					if(elem_current=="..")
						{
						ret.erase(ret.size()-1,1);
						auto pos=ret.find_last_of('/');
						if(pos==ret.npos)
							{ret.clear();}
						else
							{ret.erase(ret.find_last_of('/'))+='/';}
						}
					else
					if(elem_current!=".")
						{ret.append(elem_current)+='/';}

					elem_current.clear();
					break;

				default:
					elem_current+=*str;
				}
			++str;
			}
		ret+=elem_current;
		return std::move(  ret );
		}

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
	}

#endif
