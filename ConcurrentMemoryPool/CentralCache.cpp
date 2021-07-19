#include"CentralCache.h"
#include"PageCache.h"


//从centralCache中取span
Span* CentralCache::GetOneSpan(SpanList& list, size_t size)
{
	//1.先在CentralCache的spanlist中找还有内存的span，2种情况：
	//	a.遍历SpanList,span有内存
	//  b.span没有内存向pageCache申请内存
	Span* it = list.Begin();
	while (it != list.End())
	{
		//i.找到了就返回
		if (it->_memory)
			return it;
		//ii.没找到就遍历下一个
		it = it->_next;
	}

	// 走到这里代表着span都没有内存了，b情况，只能找pagecache
	//通过NumMovePage算需要多少页
	return pageCache.NewSpan(SizeClass::NumMovePage(size));
}

//给ThreadCache提供内存
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t size)
{
	//1.先算对应的位置
	size_t i = SizeClass::Index(size);
	//2.从centralCache中取span
	Span* span = GetOneSpan(_spanLists[i], size);

	// ...
	start = span->_memory;
	for (size_t i = 0; i < n; ++i)
	{

	}


}