//@	{"targets":[{"name":"build_info.o","type":"object"}]}

#include "./build_info.hpp"

#include <algorithm>
#include <random>

Maike::BuildId::BuildId()
{
	std::generate(
	   std::begin(m_bytes),
	   std::end(m_bytes),
	   [u = std::uniform_int_distribution<uint8_t>{0, 255}, rng = std::random_device{}]() mutable{
		return static_cast<std::byte>(u(rng));
	   });
}