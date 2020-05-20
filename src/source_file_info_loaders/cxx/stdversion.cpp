//@	{
//@	 "targets":[{"name":"stdversion.o","type":"object"}]
//@	}

#include "./stdversion.hpp"

#include <cstring>
#include <stdexcept>

Cxx::Stdversion::Stdversion(char const* name)
{
	if(name == nullptr) { m_value = -1; }
	else if(strcmp(name, "") == 0)
	{
		m_value = -1;
	}
	else if(strcmp(name, "c++98") == 0)
	{
		m_value = 199711L;
	}
	else if(strcmp(name, "c++03") == 0)
	{
		m_value = 199711L;
	}
	else if(strcmp(name, "c++11") == 0)
	{
		m_value = 201103L;
	}
	else if(strcmp(name, "c++14") == 0)
	{
		m_value = 201402L;
	}
	else if(strcmp(name, "c++17") == 0)
	{
		m_value = 201703L;
	}
	else if(strcmp(name, "c++20") == 0)
	{
		m_value = 202002L;
	}
	else
	{
		throw std::runtime_error{std::string{"Unsupported C++ standard version: "} + name};
	}
}

char const* Cxx::Stdversion::c_str() const
{
	switch(m_value)
	{
		case 199711L: return "c++03";
		case 201103L: return "c++11";
		case 201402L: return "c++14";
		case 201703L: return "c++17";
		case 202002L: return "c++20";
		default: return "";
	}
}
