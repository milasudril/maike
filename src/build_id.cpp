//@	{"targets":[{"name":"build_id.o","type":"object"}]}

#include "./build_id.hpp"

#include <algorithm>
#include <random>

Maike::BuildId::BuildId()
{
	std::generate(std::begin(m_bytes),
	              std::end(m_bytes),
	              [u = std::uniform_int_distribution<uint8_t>{0, 255},
	               rng = std::random_device{}]() mutable { return static_cast<std::byte>(u(rng)); });
}

std::string Maike::toString(BuildId const& id)
{
	auto& bytes = id.bytes();
	std::string ret;
	for(size_t k = 0; k < bytes.size(); ++k)
	{
		constexpr std::array<char, 16> val_lut{
		   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		auto val = bytes[k];
		auto msb = static_cast<int>((val & static_cast<std::byte>(0xf0)) >> 4);
		auto lsb = static_cast<int>((val & static_cast<std::byte>(0x0f)));

		ret += val_lut[msb];
		ret += val_lut[lsb];
	};
	return ret;
}