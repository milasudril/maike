//@	{"targets":[{"name":"varstring.hpp", "type":"include"}]}

#ifndef MAIKE_UTILS_VARSTRING_HPP
#define MAIKE_UTILS_VARSTRING_HPP

#include <string>

namespace Maike
{
	class Varstring
	{
	public:
		enum class Type : int
		{
			Value,
			Variable
		};

		explicit Varstring(Type type, std::string&& str): m_type{type}, m_value{type}
		{
		}

		template<class Dictionary>
		std::string const& get(Dictionary const& dict) const;

	private:
		Type m_type;
		std::string m_value;
	};

	template<class Dictionary>
	std::string const& Varstring::get(Dictionary const& dict) const
	{
		switch(m_type)
		{
			case Type::Value: return m_value;
			case Type::Variable:
			{
				auto i = dict.find(m_value);
				using std::end;
				if(i == end(dict))
				{
					std::string msg{"`"};
					msg += m_value;
					msg += "` not in dictionary";
					throw std::runtime_error{msg};
				}
				return i->second;
			}
		}
	}
}

#endif