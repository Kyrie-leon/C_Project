#pragma once

#include "Common.h"
#include "ThreadCache.h"
#include"PageCache.h"
#include<assert.h>


//为每个线程分配空间
static void* ConcurrentAlloc(size_t size)
{
	//保证多线程安全，使用TLS技术，线程安全的
	if (size > MAX_BYTES)
	{
		//PageCache,65  68,以页为单位
		size_t npage = SizeClass::RoundUp(size) >> PAGE_SHIFT;	//以页为单位取整
		Span* span = PageCache::GetInstance()->NewSpan(npage);
		span->_objsize = size;

		void* ptr = (void*)(span->_pageId << PAGE_SHIFT);
		return ptr;
	}
	else
	{
		if (tls_threadcache == nullptr)
		{
			//存在堆上，对象池解决
			tls_threadcache = new ThreadCache;
		}

		return tls_threadcache->Allocate(size);
	}
}


//释放线程空间
static void ConcurrentFree(void* ptr)
{
	//获取对象到span的映射
	Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);
	size_t size = span->_objsize;

	if (size > MAX_BYTES)
	{
		// PageCache
		PageCache::GetInstance()->ReleaseSpanToPageCahce(span);
	}
	else
	{
		assert(tls_threadcache);
		tls_threadcache->Deallocate(ptr, size);
	}
}