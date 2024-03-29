#pragma once
#include"Common.h"

class CentralCache
{
public:
	static CentralCache* GetInstance()
	{
		return &_sInst;
	}

	//从中心缓存获取一定数量的对象给thread cache
	size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t byte_size);

	//从SpanList或者page cache中获取一个span
	Span* GetOneSpan(SpanList& list, size_t byte_size);

	// 将一定数量的对象释放到span跨度
	void ReleaseListToSpans(void* start, size_t byte_size);
private:
	SpanList _spanLists[NLISTS];	//按对齐方式映射，把每一个span挂起来
private:
	CentralCache()
	{}
	//C++98拷贝构造只声明不定义
	CentralCache(const CentralCache&) = delete;

	static CentralCache _sInst;
};
