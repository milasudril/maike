//@	[
//@		[
//@		"posix"
//@			,{"targets":[{"name":"thread.o","type":"object"}]}
//@		]
//@	]
#include "thread.hpp"
#include <pthread.h>

using namespace Maike;

static void* thread_entry(void* thread)
	{
	reinterpret_cast<ThreadBase*>(thread)->run();
	return nullptr;
	}

ThreadBase::ThreadBase()
	{
	static_assert(sizeof(m_handle)>=sizeof(pthread_t),"Handle type is too small");
	}

void ThreadBase::start()
	{
	pthread_create(reinterpret_cast<pthread_t*>(&m_handle)
		,NULL,thread_entry,this);
	}

ThreadBase::~ThreadBase()
	{
	pthread_join(m_handle,NULL);
	}
