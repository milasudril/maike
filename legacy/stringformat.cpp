//@	{"targets":[{"name":"stringformat.o","type":"object"}]}

#include "stringformat.hpp"
#include "variant.hpp"
#include "twins.hpp"
#include <algorithm>
#include <cstring>
#include <cstdio>

using namespace Maike;

template<class T>
T string2uint(Twins<const char*> str_range)
	{
	T ret=0;
	auto digit=1;
	while(str_range.first!=str_range.second)
		{
		--str_range.second;
		auto ch_in=*str_range.second;
		if( !(ch_in>='0'&& ch_in<='9') )
			{return ret;}
		ret+=digit*(ch_in - '0');
		digit*=10;
		}
	return ret;
	}


static const char* digits="0123456789ABCDEF";

template<class T>
static char* convert_impl(char* buffer,typename std::make_unsigned<T>::type x
	,unsigned int radix) noexcept
	{
	auto begin=buffer;
	while(x)
		{
		typename std::make_unsigned<T>::type rem=x%radix;
		x/=radix;
		*buffer = digits[rem];
		++buffer;
		}
	if(buffer==begin)
		{
		*buffer=digits[0];
		++buffer;
		}
	*buffer='\0';
	std::reverse(begin,buffer);
	return buffer;
	}

template<class T>
static char* convert(char* result,T value,unsigned int radix) noexcept
	{
	if(std::is_signed<T>::value)
		{
		if(value < 0)
			{
			*result='-';
			++result;
			value=-value;
			}

		typename std::make_unsigned<T>::type x=value;
		return convert_impl<T>(result,x,radix);
		}
	else
		{return convert_impl<T>(result,value,radix);}
	}

///Pointer

static size_t format(const Twins<char*>& range,const void* ptr
	,const Twins<const char*>&) noexcept
	{
	size_t N=range.second-range.first;
	auto pos=range.first;
	if(ptr==nullptr)
		{
		auto n=std::min(static_cast<size_t>(5),N);
		memcpy(pos,"null",n);
		return n;
		}
	else
		{
		char buffer[2 + 2*sizeof(ptr)+1]={"0x"};
		auto pos_out=convert(buffer+2,reinterpret_cast<uintptr_t>(ptr),16);
		auto n=std::min(static_cast<size_t>(pos_out-buffer),N);
		memcpy(pos,buffer,n);
		return n;
		}
	return 0;
	}

///String constant

static size_t format(const Twins<char*>& range,const char* cstr
	,const Twins<const char*>&) noexcept
	{
	auto ptr=range.first;
	auto ptr_end=range.second;
	while(*cstr!='\0' && ptr!=ptr_end)
		{
	//	TODO: Add support for various transformation filters: UPPER/lowecase... .
		*ptr=*cstr;
		++ptr;
		++cstr;
		}
	return static_cast<size_t>(ptr-range.first);
	}

///Integer printing stuff

static unsigned int radixGet(const Twins<const char*>& argstring)
	{
	auto args_begin=argstring.first;
	auto args_end=argstring.second;
	enum class State:unsigned int{NORMAL,ARGS_BEGIN,RADIX,DONE};
	auto state=State::NORMAL;
	auto radix=10u;
	const char* radix_pos=nullptr;
	while(args_begin!=args_end && state!=State::DONE)
		{
		auto ch_in=*args_begin;
		switch(state)
			{
			case State::NORMAL:
				switch(ch_in)
					{
					case 'b':
						radix=2;
						state=State::DONE;
						break;
					case 'o':
						radix=8;
						state=State::DONE;
						break;
					case 'd':
						radix=10;
						state=State::DONE;
						break;
					case 'h':
						radix=16;
						state=State::DONE;
						break;
					case ':':
						state=State::ARGS_BEGIN;
						break;
					default:
						return radix;
					}
				break;

			case State::ARGS_BEGIN:
				if(ch_in>='0' && ch_in<='9')
					{
					radix_pos=args_begin;
					state=State::RADIX;
					}
				else
					{return radix;}
				break;

			case State::RADIX:
				if(!(ch_in>='0' && ch_in<='9'))
					{
					state=State::DONE;
					radix=string2uint<unsigned int>(Twins<const char*>(radix_pos,args_begin));
					}
				break;

			case State::DONE:
				break;
			}
		++args_begin;
		}
	if(args_begin==args_end && radix_pos!=nullptr)
		{radix=string2uint<unsigned int>(Twins<const char*>(radix_pos,args_begin));}
	return std::max(2u,std::min(radix,16u));
	}

static size_t format(const Twins<char*>& range,int64_t x
	,const Twins<const char*>& argstring) noexcept
	{
	auto radix=radixGet(argstring);

	char buffer[1 + 8*sizeof(x) + 1];
	auto pos=convert(buffer,x,radix);
	size_t N=range.second-range.first;
	auto n=std::min(static_cast<size_t>(pos-buffer),N);
	memcpy(range.first,buffer,n);
	return n;
	}

static size_t format(const Twins<char*>& range,uint64_t x
	,const Twins<const char*>& argstring) noexcept
	{
	auto radix=radixGet(argstring);

	char buffer[8*sizeof(x) + 1];
	auto pos=convert(buffer,x,radix);
	size_t N=range.second-range.first;
	auto n=std::min(static_cast<size_t>(pos-buffer),N);
	memcpy(range.first,buffer,n);
	return n;
	}


///Char

static size_t format(const Twins<char*>& range,char x
	,const Twins<const char*>& argstring) noexcept
	{
	size_t N=range.second-range.first;
	if(N)
		{
		N=argstring.second-argstring.first;
		if(N)
			{
			if(argstring.first[0]=='n')
				{
				return format(range,static_cast<int64_t>(x)
					,Twins<const char*>(argstring.first + 1,argstring.second));
				}
			}
		range.first[0]=x;
		return 1;
		}
	return 0;
	}


///Bool

static size_t format(const Twins<char*>& range,bool x
	,const Twins<const char*>&) noexcept
	{
	auto pos=range.first;
	size_t N=range.second-range.first;
	if(x)
		{
		auto n=std::min(static_cast<size_t>(4),N);
		memcpy(pos,"true",n);
		return n;
		}
	else
		{
		auto n=std::min(static_cast<size_t>(5),N);
		memcpy(pos,"false",n);
		return n;
		}
	}


///Floating point stuff

static std::pair<char,unsigned int>
floatParams(const Twins<const char*>& argstring
	,unsigned int digits_default,unsigned int digits_max)  noexcept
	{
	auto n_digits=digits_default;
	auto separator='.';
	enum class State:unsigned int{NORMAL,DIGITS};

	auto args_begin=argstring.first;
	auto args_end=argstring.second;
	auto state=State::NORMAL;
	const char* digits_pos=nullptr;
	while(args_begin!=args_end)
		{
		auto ch_in=*args_begin;
		switch(state)
			{
			case State::NORMAL:
				state=State::DIGITS;
				if(ch_in!=',' && ch_in!='.')
					{return {separator,n_digits};}
				separator=ch_in;
				digits_pos=args_begin + 1;
				break;
			case State::DIGITS:
				if(!(ch_in>='0' && ch_in<='9'))
					{
					auto n_digits=string2uint<unsigned int>(Twins<const char*>(digits_pos,args_begin));
					return
						{
						separator,std::max(1u,std::min(n_digits,digits_max))
						};
					}
				break;
			}
		++args_begin;
		}

	if(args_begin==args_end && digits_pos!=nullptr)
		{n_digits=string2uint<unsigned int>(Twins<const char*>(digits_pos,args_begin));}
	return {separator,std::max(1u,std::min(n_digits,digits_max))};
	}

static size_t format(const Twins<char*>& range,double x
	,const Twins<const char*>& argstring) noexcept
	{
	auto params=floatParams(argstring,15,16);
	char buffer[32];
	char format[6];
	sprintf(format,"%%.%ug",params.second);
	sprintf(buffer,format,x);

	auto pos=range.first;
	auto pos_end=range.second;
	auto ptr_source=buffer;
	while(pos!=pos_end && *ptr_source!='\0')
		{
		auto ch_in=*ptr_source;
		if(ch_in==',' || ch_in=='.')
			{ch_in=params.first;}
		*pos=ch_in;
		++ptr_source;
		++pos;
		}
	return pos - range.first;
	}


static size_t format(const Twins<char*>& range,float x
	,const Twins<const char*>& argstring) noexcept
	{
	auto params=floatParams(argstring,7,8);
	char buffer[32];
	char format[6];
	sprintf(format,"%%.%ug",params.second);
	sprintf(buffer,format,x);

	auto pos=range.first;
	auto pos_end=range.second;
	auto ptr_source=buffer;
	while(pos!=pos_end && *ptr_source!='\0')
		{
		auto ch_in=*ptr_source;
		if(ch_in==',' || ch_in=='.')
			{ch_in=params.first;}
		*pos=ch_in;
		++ptr_source;
		++pos;
		}
	return pos - range.first;
	}



static char* process(const Twins<char*>& range,const Variant& var
	,const Twins<const char*>& argstring) noexcept
	{
	auto pos=range.first;
	switch(var.typeGet())
		{
		case Variant::POINTER:
			pos+=format(range,static_cast<const void*>(var),argstring);
			break;
		case Variant::STRING:
			pos+=format(range,static_cast<const char*>(var),argstring);
			break;
		case Variant::INT:
			pos+=format(range,static_cast<int64_t>(var),argstring);
			break;
		case Variant::UINT:
			pos+=format(range,static_cast<uint64_t>(var),argstring);
			break;
		case Variant::CHAR:
			pos+=format(range,static_cast<char>(var),argstring);
			break;
		case Variant::BOOL:
			pos+=format(range,static_cast<bool>(var),argstring);
			break;
		case Variant::DOUBLE:
			pos+=format(range,static_cast<double>(var),argstring);
			break;
		case Variant::FLOAT:
			pos+=format(range,static_cast<float>(var),argstring);
			break;

		default:
			{
		//	User-specified type...
			auto obj=std::pair<const void*,const Variant::Formatter*>(var);
			pos+=obj.second->format(range,obj.first,argstring);
			}
		}
	return pos;
	}

void Maike::format(const Twins<char*>& buffer,const char* format_string
	,const Twins<const Variant*>& args)
	{
	auto write_pos=buffer.first;
	auto write_end=buffer.second;

	enum class State:unsigned int
		{
		 NORMAL
		,PLACEHOLDER_BEGIN
		,PLACEHOLDER_INDEX
		,PLACEHOLDER_ARGSTRING
		};

	auto state=State::NORMAL;
	const char* numpos=nullptr;
	const char* argpos=nullptr;
	size_t index_current=0;
	size_t N=args.second-args.first;

	while(write_pos!=write_end && *format_string!='\0')
		{
		auto ch_in=*format_string;
		switch(state)
			{
			case State::NORMAL:
				switch(ch_in)
					{
					case '#':
						state=State::PLACEHOLDER_BEGIN;
						break;
					default:
						*write_pos=ch_in;
						++write_pos;
					}
				break;

			case State::PLACEHOLDER_BEGIN:
				if(ch_in>='0' && ch_in<='9')
					{
					numpos=format_string;
					state=State::PLACEHOLDER_INDEX;
					}
				else
					{
					*write_pos=ch_in;
					++write_pos;
					state=State::NORMAL;
					}
				break;

			case State::PLACEHOLDER_INDEX:
				if(!(ch_in>='0'&& ch_in<='9'))
					{
					index_current=string2uint<size_t>(Twins<const char*>(numpos,format_string));
					if(index_current>=N) //Ignore out-of-bounds index
						{state=State::NORMAL;}
					else
					if(ch_in==';')
						{
						state=State::NORMAL;
						write_pos=process(Twins<char*>(write_pos,write_end)
							,args.first[index_current],Twins<const char*>(format_string,format_string));
						}
					else
						{
						state=State::PLACEHOLDER_ARGSTRING;
						argpos=format_string;
						}
					}
				break;

			case State::PLACEHOLDER_ARGSTRING:
				if(ch_in==';')
					{
					state=State::NORMAL;
					write_pos=process(Twins<char*>(write_pos,write_end)
						,args.first[index_current],Twins<const char*>(argpos,format_string));
					}
				break;
			}
		++format_string;
		}
	if(write_pos!=write_end)
		{
		*write_pos='\0';
		}
	*(write_end - 1)='\0';
	}
