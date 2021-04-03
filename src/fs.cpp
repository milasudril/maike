//@	{
//@	  "targets":[{"name":"fs.o","type":"object"}]
//@	 }

#include "./fs.hpp"

bool Maike::newer(fs::path const& a, fs::path const& b)
{
	std::error_code ec_a{};
	std::error_code ec_b{};

	auto t_a = last_write_time(a, ec_a);
	auto t_b = last_write_time(b, ec_b);

	if(ec_a && ec_b)
	{
		std::string msg{"Neither "};
		msg += a;
		msg += " or ";
		msg += b;
		msg += " exists";

		throw std::runtime_error{std::move(msg)};
	}

	if(ec_a)
	{ return false; }

	if(ec_b)
	{ return true; }

	return t_a > t_b;
}