#pragma once
#include"Common.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_sInst;
	}
	//向系统申请k页内存挂到自由链表
	void* SystemAllocPage(size_t k);
	
	//申请一个新的span
	Span* NewSpan(size_t k);

	// 获取从对象到span的映射
	Span* MapObjectToSpan(void* obj);
	// 释放空闲span回到Pagecache，并合并相邻的span
	void ReleaseSpanToPageCahce(Span* span);


private:
	SpanList _spanList[NPAGES];	//按页数映射
	std::unordered_map<PageID, Span*> _idSpanMap;	//页号和内存地址的映射

	std::recursive_mutex _mtx;

private:
	PageCache()
	{}

	PageCache(const PageCache&) = delete;
	//单例
	static PageCache _sInst;
};