#include"CentralCache.h"
#include"PageCache.h"

CentralCache CentralCache::_sInst;

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
		if (it->_list)
			return it;
		//ii.没找到就遍历下一个
		it = it->_next;
	}

	// 走到这里代表着span都没有内存了，b情况，只能找pagecache
	//通过NumMovePage算需要多少页
	return PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(size));
}

//给ThreadCache提供内存
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t size)
{
	//1.先算对应的位置
	size_t index = SizeClass::Index(size);
	//2.从centralCache中取span
	Span* span = GetOneSpan(_spanLists[index], size);

	// 切分span,有可能span只有list，所以让start和end都指向span->_list
	start = span->_list;
	void* prev = start;
	void* cur = start;
	size_t i = 1;	//end走n-1步因此i=1
	//end往后走n-1步切出n个内存
	//注意：如果span不能提供n个内存，有多少给多少因此加一个end不为空的条件
	for (;i <= n && cur != nullptr; ++i)
	{
		prev = cur;
		cur = NextObj(cur);
		span->_usecount++;
	}

	//_list指向切分后的内存的首个元素,分出去的end置空
	span->_list = cur;
	end = prev;
	NextObj(prev) = nullptr;

	//真实返回的个数i-1
	return i-1;
}

//list还给span
void CentralCache::ReleaseListToSpans(void* start, size_t byte_size)
{
	//1.计算映射下标
	size_t i = SizeClass::Index(byte_size);

	while (start)
	{
		//保存下一个结点
		void* next = NextObj(start);
		//找start属于哪个span
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);
		//头插
		NextObj(start) = span->_list;
		span->_list = start;
		//span的usecount减1
		span->_usecount--;
		//当usecount为0时说明span切除取鼎的大块内存都还回来了
		if (span->_usecount == 0)
		{
			//先从span接下来
			_spanLists[i].Erase(span);
			span->_list = nullptr;
			//还给pagecache
			PageCache::GetInstance()->ReleaseSpanToPageCahce(span);
		}

		start = next;
	}
}