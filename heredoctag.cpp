//@	{"targets":[{"name":"heredoctag","type":"application"}]}

#include <cstdio>
#include <cmath>

int main()
	{
	FILE* src=fopen("/dev/random","rb");
	static constexpr auto N_bits=256;
 	static constexpr auto bits_per_byte=std::log2(127 - 35);
	static constexpr auto N_bytes=static_cast<size_t>(std::ceil(N_bits/bits_per_byte));
	size_t n=N_bytes;
	while(n!=0)
		{
		auto ch_in=getc(src);
		if((ch_in>' ' && ch_in<127 && n!=N_bytes && ch_in!='\'')
			|| (ch_in>' ' && ch_in<127 && n==N_bytes && ch_in!='-'))
			{
			putchar(ch_in);
			--n;
			}
		}
	fclose(src);
	return 0;
	}
