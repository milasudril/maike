//@	[
//@		[
//@		"posix"
//@			,{
//@			"targets":
//@				[{
//@				 "name":"thread.o","type":"object"
//@				,"dependencies":[{"ref":"pthread","rel":"external"}]
//@				}]
//@			}
//@		]
//@	]

#include "thread.hpp"
#include <pthread.h>
using namespace Maike;

ThreadBase::ThreadBase()
	{
	static_assert(sizeof(m_handle)>=sizeof(pthread_t),"Handle type is too small");
	}

static void* thread_entry(void* thread)
	{
	auto obj=reinterpret_cast<ThreadBase*>(thread);
	obj->run();
	return nullptr;
	}

void ThreadBase::start()
	{
	pthread_t thread;
	pthread_create(&thread,NULL,thread_entry,this);
	m_handle=thread;
	}

ThreadBase::~ThreadBase()
	{}

void ThreadBase::synchronize()
	{
	pthread_join(m_handle,NULL);
	}
