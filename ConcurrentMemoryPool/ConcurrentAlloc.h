#pragma once

#include "Common.h"
#include "ThreadCache.h"
#include<assert.h>


//Ϊÿ���̷߳���ռ�
void* ConcurrentAlloc(size_t size)
{
	//��֤���̰߳�ȫ��ʹ��TLS�������̰߳�ȫ��
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


//�ͷ��߳̿ռ�
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