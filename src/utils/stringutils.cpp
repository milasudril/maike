//@	 {"targets":[{"name":"stringutils.o","type":"object"}]}

#include "./stringutils.hpp"

#include <algorithm>

std::string Maike::toString(std::vector<std::byte> const& data)
{
	std::string ret;
	ret.reserve(data.size());
	std::transform(std::begin(data), std::end(data), std::back_inserter(ret), [](std::byte val) {
		return static_cast<char>(val); // TODO: What about ascii nul?
	});
	return ret;
}

std::string Maike::trim(std::string const& src)
{
	auto not_whitespace = [](auto ch_in) { return !(ch_in >= 0 && ch_in <= ' '); };

	auto i = std::find_if(std::begin(src), std::end(src), not_whitespace);
	auto j = std::find_if(std::rbegin(src), std::rend(src), not_whitespace);
	if(j == std::rend(src) || i == std::end(src)) { return std::string{}; }

	return std::string{i, j.base()};
}