//@	{"targets":[{"name":"pathutils.o","type":"object"}]}

#include "pathutils.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include "errormessage.hpp"

using namespace Maike;

PRIVATE std::string Maike::directoryNormalize(const char* str)
	{
	std::string ret;
	std::string elem_current;
	bool up_more=0;
	const char* base=str;
	while(*str!='\0')
		{
		switch(*str)
			{
			case '/':
				if(elem_current=="..")
					{
					if(ret.size()==0 || up_more)
						{
						if(*base=='/')
							{exceptionRaise(ErrorMessage("#0; is a malformed path.",{base}));}
						ret+=elem_current;
						ret+='/';
						up_more=1;
						}
					else
						{
						ret.erase(ret.size()-1,1);
						auto pos=ret.find_last_of('/');
						if(pos==ret.npos)
							{ret.clear();}
						else
							{ret.erase(ret.find_last_of('/'))+='/';}
						}
					}
				else
				if(elem_current!=".")
					{
					ret.append(elem_current)+='/';
					up_more=0;
					}
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