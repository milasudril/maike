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

	Maike::fs::path getExecPrefix()
	{
		return read_symlink(Maike::fs::path{"/proc/self/exe"}).parent_path().parent_path();
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

Maike::fs::path const& Maike::execPrefix()
{
	static auto ret = getExecPrefix();

	return ret;
}


std::string Maike::getFullExtension(fs::path const& path)
{
	std::string extension;
	for(auto p = path; !p.extension().empty(); p = p.stem())
	{
		extension = std::string{p.extension()} + extension;
	}
	return extension;
}