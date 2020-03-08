//@	 {"targets":[{"name":"stringutils.o","type":"object"}]}

#include "./stringutils.hpp"

#include <algorithm>

std::string Maike::toString(std::vector<std::byte> const& data)
{
	std::string ret;
	std::transform(std::begin(data), std::end(data), std::back_inserter(ret), [](std::byte val) {
		return static_cast<char>(val); // TODO: What about ascii nul?
	});
	return ret;
}
