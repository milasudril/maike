//@	{"targets":[{"name":"id.o","type":"object"}]}

#include "./id.hpp"

#include <algorithm>
#include <random>

Maike::Build::Id::Id()
{
	std::generate(std::begin(m_bytes),
	              std::end(m_bytes),
	              [u = std::uniform_int_distribution<uint8_t>{0, 255},
	               rng = std::random_device{}]() mutable { return static_cast<std::byte>(u(rng)); });
}

namespace
{
	std::byte to_nibble(char ch_in)
	{
		if(ch_in >= '0' && ch_in <= '9') { return static_cast<std::byte>(ch_in - '0'); }

		auto ch_in_masked = ch_in & (~0x20);
		if(ch_in_masked >= 'A' && ch_in_masked <= 'F')
		{ return static_cast<std::byte>(10 + ch_in_masked - 'A'); }

		throw std::runtime_error{std::string{"Invalid BuildId: `"} + ch_in
		                         + "` is not a hexadecimal digit"};
	}
}

Maike::Build::Id::Id(char const* cstr): m_bytes{}
{
	size_t k = 0;
	std::byte val;
	while(true)
	{
		auto ch_in = *cstr;
		if(ch_in == '\0' || k == 64) { return; }

		auto nibble = to_nibble(ch_in);

		if(k % 2 == 1)
		{
			val |= nibble;
			m_bytes[k / 2] = val;
		}
		else
		{
			val = nibble << 4;
		}
		++cstr;
		++k;
	}
}

std::string Maike::Build::toString(Id const& id)
{
	auto& bytes = id.bytes();
	std::string ret;
	for(size_t k = 0; k < bytes.size(); ++k)
	{
		constexpr std::array<char, 16> val_lut{
		   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
		auto val = bytes[k];
		auto msb = static_cast<int>((val & static_cast<std::byte>(0xf0)) >> 4);
		auto lsb = static_cast<int>((val & static_cast<std::byte>(0x0f)));

		ret += val_lut[msb];
		ret += val_lut[lsb];
	};
	return ret;
}