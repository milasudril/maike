//@	{
//@	 "targets":[{"name":"property.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_PROPERTY_HPP
#define MAIKE_DB_PROPERTY_HPP

#include <string>

namespace Maike::Db
{
	class Property
	{
	public:
		explicit Property(std::string&& name): m_name{std::move(name)} {};

		explicit Property(std::string&& name, std::string&& value):
		   m_name{std::move(name)}, m_value{std::move(value)} {};

		std::string const& name() const
		{
			return m_name;
		}

		std::string const& value() const
		{
			return m_value;
		}

	private:
		std::string m_name;
		std::string m_value;
	};
}

#endif