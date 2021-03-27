//@	{
//@	 "targets":[{"name":"versionnumber.o","type":"object"}]
//@	}

#include "versionnumber.hpp"
#include <cstring>
#include <cstdlib>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
inline static uint16_t makeWord(uint16_t x)
{
	return __builtin_bswap16(x);
}

inline static int64_t makeWord(int64_t x)
{
	return __builtin_bswap64(x);
}

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

inline static uint16_t makeWord(uint16_t x)
{
	return x;
}

inline static int64_t makeWord(int64_t x)
{
	return x;
}

#else
#error "Unknown byte order"
#endif

int64_t Maike::version(const char* version_string) noexcept
{
	union
	{
		uint16_t parts[4];
		int64_t number;
	} version;
	version.number = 0;

	static constexpr size_t N_max = 4 * (5 + 1); //	5 decimal places + one delimiter
	char buff_temp[N_max];
	strncpy(buff_temp, version_string, N_max);
	buff_temp[N_max - 1] = '\0';

	auto k = 0;
	char* pos;
	auto ptr = strtok_r(buff_temp, ".", &pos);
	while(ptr != NULL && k != 4)
	{
		version.parts[k] = makeWord(static_cast<uint16_t>(atoi(ptr)));
		ptr = strtok_r(NULL, ".", &pos);
		++k;
	}
	version.number = makeWord(version.number);
	return version.number;
}
