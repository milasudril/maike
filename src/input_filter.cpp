//@	{
//@	  "targets":[{"name":"input_filter.o","type":"object"}]
//@	}

#include "./input_filter.hpp"

Maike::InputFilter::InputFilter()
{
	source(std::vector<std::string>{R"_(^\.)_", R"_(^__)_"});
}

Maike::InputFilter::InputFilter(KeyValueStore::ArrayRefConst obj)
{
	std::vector<std::string> input_filters_src;
	input_filters_src.reserve(obj.size());
	std::transform(std::begin(obj),
	               std::end(obj),
	               std::back_inserter(input_filters_src),
	               [](auto const& item) { return item.template as<char const*>(); });

	source(std::move(input_filters_src));
}

Maike::InputFilter& Maike::InputFilter::source(std::vector<std::string>&& val)
{
	std::vector<std::regex> filters_tmp;
	filters_tmp.reserve(val.size());
	std::transform(
	   std::begin(val), std::end(val), std::back_inserter(filters_tmp), [](auto const& item) {
		   return std::regex{item, std::regex_constants::basic};
	   });

	m_input_filters_src = std::move(val);
	m_input_filters = std::move(filters_tmp);
	return *this;
}

bool Maike::InputFilter::match(char const* str) const
{
	return std::any_of(std::begin(m_input_filters),
	                   std::end(m_input_filters),
	                   [str](auto const& regex) { return regex_search(str, regex); });
}

Maike::KeyValueStore::JsonHandle Maike::toJson(InputFilter const& cfg)
{
	auto const& input_filters = cfg.source();

	return KeyValueStore::Array{std::begin(input_filters), std::end(input_filters)}.takeHandle();
}