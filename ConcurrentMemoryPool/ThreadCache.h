#ifndef __THREAD_CACHE_H
#define __THREAD_CACHE_H


#include"Common.h"

class ThreadCache
{
public:
	//�����ڴ����
	void* Allocate(size_t size);

	//�ͷ��ڴ����
	void Deallocate(void* ptr, size_t size);

	//�����Ļ����ȡ����
	void* FetchFromCentralCache(size_t index, size_t size);

	//�ͷŶ����������������ڴ���յ�Central Cache
	void ListTooLong(FreeList& list, size_t size);
private:
	FreeList _freeLists[NLISTS];	//��������
};

//thread local storage(TLS)��ÿ�������ж����ĵ�ַ�ռ䣬�������������
static __declspec(thread) ThreadCache* tls_threadcache = nullptr;

#endif