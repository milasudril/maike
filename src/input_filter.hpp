//@	{
//@	  "targets":[{"name":"input_filter.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"input_filter.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_INPUTFILTER_HPP
#define MAIKE_INPUTFILTER_HPP

#include "src/key_value_store/array.hpp"

#include <regex>
#include <vector>

namespace Maike
{
	class InputFilter
	{
	public:
		InputFilter();

		explicit InputFilter(KeyValueStore::ArrayRefConst obj);

		std::vector<std::string> const& source() const
		{
			return m_input_filters_src;
		}

		std::vector<std::regex> const& regex() const
		{
			return m_input_filters;
		}

		InputFilter& source(std::vector<std::string>&& val);

		bool match(char const* str) const;

	private:
		std::vector<std::regex> m_input_filters;
		std::vector<std::string> m_input_filters_src;
	};

	KeyValueStore::JsonHandle toJson(InputFilter const& cfg);

	inline bool operator==(InputFilter const& a, InputFilter const& b)
	{
		return a.source() == b.source();
	}

	inline bool operator!=(InputFilter const& a, InputFilter const& b)
	{
		return !(a == b);
	}

	inline auto fromJson(KeyValueStore::Empty<InputFilter>, KeyValueStore::JsonRefConst ref)
	{
		return InputFilter{ref.as<KeyValueStore::ArrayRefConst>()};
	}
}

#endif