//@	{"targets":[{"name":"varstring.hpp", "type":"include"}]}

#ifndef MAIKE_UTILS_VARSTRING_HPP
#define MAIKE_UTILS_VARSTRING_HPP

#include <string>
#include <vector>

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

		explicit Varstring(Type type, std::string&& val): m_type{type}
		{
			m_values.push_back(std::move(val));
		}

		explicit Varstring(std::vector<std::string>&& vals):
		   m_type{Type::Value}, m_values{std::move(vals)}
		{
		}

		template<class Dictionary>
		std::vector<std::string> const& get(Dictionary const& dict) const;

	private:
		Type m_type;
		std::vector<std::string> m_values;
	};

	template<class Dictionary>
	std::vector<std::string> const& Varstring::get(Dictionary const& dict) const
	{
		switch(m_type)
		{
			case Type::Value: return m_values;
			case Type::Variable:
			{
				auto i = dict.find(m_values[0]);
				using std::end;
				if(i == end(dict))
				{
					std::string msg{"`"};
					msg += m_values[0];
					msg += "` not in dictionary";
					throw std::runtime_error{msg};
				}
				return i->second;
			}
		}
	}
}

#endif