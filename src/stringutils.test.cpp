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

std::string Maike::toString(std::chrono::time_point<std::chrono::system_clock> time)
{
	auto timet = std::chrono::system_clock::to_time_t(time);


	tm res{};
	gmtime_r(&timet, &res);

	char buffer[64];
	sprintf(buffer, "%d-%02d-%02d %d:%02d:%02d UTC", res.tm_year + 1900, res.tm_mon + 1, res.tm_mday, res.tm_hour, res.tm_min, res.tm_sec);

	return buffer;
}
