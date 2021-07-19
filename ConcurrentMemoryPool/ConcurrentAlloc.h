#pragma once

#include "Common.h"
#include "ThreadCache.h"
#include<assert.h>


//为每个线程分配空间
void* ConcurrentAlloc(size_t size)
{
	//保证多线程安全，使用TLS技术，线程安全的
	if (size > MAX_BYTES)
	{

	}
	else
	{
		if (tls_threadcache == nullptr)
		{
			tls_threadcache = new ThreadCache;
		}

		return tls_threadcache->Allocate(size);
	}
}


//释放线程空间
void ConcurrentFree(void* ptr, size_t size)
{
	assert(tls_threadcache);

	if (size > MAX_BYTES)
	{
		// PageCache
	}
	else
	{
		tls_threadcache->Deallocate(ptr, size);
	}
}