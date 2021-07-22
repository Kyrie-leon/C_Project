#ifndef __THREAD_CACHE_H
#define __THREAD_CACHE_H


#include"Common.h"

class ThreadCache
{
public:
	//申请内存对象
	void* Allocate(size_t size);

	//释放内存对象
	void Deallocate(void* ptr, size_t size);

	//从中心缓存获取对象
	void* FetchFromCentralCache(size_t index, size_t size);

	//释放对象后，链表过长，将内存回收到Central Cache
	void ListTooLong(FreeList& list, size_t size);
private:
	FreeList _freeLists[NLISTS];	//自由链表
};

//thread local storage(TLS)，每个进程有独立的地址空间，解决互斥锁问题
static __declspec(thread) ThreadCache* tls_threadcache = nullptr;

#endif