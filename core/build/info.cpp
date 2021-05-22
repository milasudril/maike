//@	{
//@	 "target":{"name":"info.o"}
//@	}

#include "./info.hpp"

#include <sys/utsname.h>

Maike::Build::Info::Info(): m_source_dir{"."}, m_target_dir{"__targets"}
{
	utsname name;
	uname(&name);
	m_build_host = name.nodename;
}