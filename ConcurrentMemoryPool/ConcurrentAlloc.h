#pragma once

#include "Common.h"
#include "ThreadCache.h"
#include"PageCache.h"
#include<assert.h>


//Ϊÿ���̷߳���ռ�
static void* ConcurrentAlloc(size_t size)
{
	//��֤���̰߳�ȫ��ʹ��TLS�������̰߳�ȫ��
	if (size > MAX_BYTES)
	{
		//PageCache,65  68,��ҳΪ��λ
		size_t npage = SizeClass::RoundUp(size) >> PAGE_SHIFT;	//��ҳΪ��λȡ��
		Span* span = PageCache::GetInstance()->NewSpan(npage);
		span->_objsize = size;

		void* ptr = (void*)(span->_pageId << PAGE_SHIFT);
		return ptr;
	}
	else
	{
		if (tls_threadcache == nullptr)
		{
			//���ڶ��ϣ�����ؽ��
			tls_threadcache = new ThreadCache;
		}

		return tls_threadcache->Allocate(size);
	}
}


//�ͷ��߳̿ռ�
static void ConcurrentFree(void* ptr)
{
	//��ȡ����span��ӳ��
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