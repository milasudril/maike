//@	{
//@	  "targets":[{"name":"fs.o","type":"object"}]
//@	 }

#include "./fs.hpp"

namespace
{
	[[noreturn]] void filesDoNotExist(Maike::fs::path const& a, Maike::fs::path const& b)
	{
		std::string msg{"Neither "};
		msg += a;
		msg += " or ";
		msg += b;
		msg += " exist";

		throw std::runtime_error{std::move(msg)};
	}
}

bool Maike::isNewer(fs::path const& a, fs::path const& b)
{
	std::error_code ec_a{};
	std::error_code ec_b{};

	auto t_a = last_write_time(a, ec_a);
	auto t_b = last_write_time(b, ec_b);

	if(ec_a && ec_b) { filesDoNotExist(a, b); }

	if(ec_a) { return false; }

	if(ec_b) { return true; }

	return t_a > t_b;
}

bool Maike::isOlder(fs::path const& a, fs::path const& b)
{
	std::error_code ec_a{};
	std::error_code ec_b{};

	auto t_a = last_write_time(a, ec_a);
	auto t_b = last_write_time(b, ec_b);

	if(ec_a && ec_b) { filesDoNotExist(a, b); }

	if(ec_a) { return true; }

	if(ec_b) { return false; }

	return t_a < t_b;
}