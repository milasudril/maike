//@	[
//@		[
//@		"linux"
//@			,{"targets":[{"name":"filein.o","type":"object"}]}
//@		]
//@	]

#include "sysvars.hpp"
#include <sys/utsname.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>

void Maike::sysvarsLoad()
	{
	utsname sysname;
	uname(&sysname);

	union
		{
		uint16_t parts[4];
		uint64_t number;
		} version;
	version.number=0;
	auto k=0;
	char* pos;
	auto ptr=strtok_r(sysname.release,".",&pos);
	while(ptr!=NULL && k!=4)
		{
		version.parts[k]=__builtin_bswap16( static_cast<uint16_t>(atoi(ptr)));
		ptr=strtok_r(NULL,".",&pos);
		++k;
		}
	version.number=__builtin_bswap64(version.number);

	printf("%016lx\n",version.number);
	}
