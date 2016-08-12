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
	while(true)
		{
		auto ch_in=*str;
		switch(ch_in)
			{
			case '\0':
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
					if(ch_in=='\0')
						{return std::move(ret);}
					}
				else
				if(elem_current==".")
					{
					if(ch_in=='\0')
						{
						if(ret.size()==0)
							{return std::string(".");}
						auto pos=ret.find_last_of('/');
						if(pos==std::string::npos)
							{return std::move(ret);}
						return ret.substr(0,pos);
						}
					}
				else
					{
					ret.append(elem_current);
					if(ch_in=='\0')
						{return ret.size()?std::move(ret):std::string(".");}
					ret+='/';
					up_more=0;
					}
				
				elem_current.clear();
				break;

			default:
				elem_current+=ch_in;
			}
		++str;
		}
	return std::string(".");
	}

PRIVATE std::string Maike::rootStrip(const std::string& path,const char* root)
	{
	auto ptr=path.data();
	auto ch_root_prev='/';
	std::string ret;
	while(true)
		{
		auto ch_in=*ptr;
		auto ch_root=*root;
		if(ch_in=='\0')
			{
			ret=std::string(ptr);
			break;
			}
		if(ch_root=='\0' && ch_root_prev!='/')
			{
			ret=std::string(ptr + 1);
			break;
			}
		if(ch_in!=ch_root)
			{
			ret=std::string(ptr);
			break;
			}
		ch_root_prev=ch_root;
		++ptr;
		++root;
		}
	return ret.size()?std::move(ret):std::string(".");
	}