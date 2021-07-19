#include"PageCache.h"

//向系统申请k页内存
void* PageCache::SystemAllocPage(size_t k)
{

}

//从PageCache中申请一个span出来
Span* PageCache::NewSpan(size_t k)
{
	//1.先找PageCache中k页的spanlist是否为空
	//   a.不为空直接返回即可
	if (!_spanList[k].Empty())
	{
		//返回给CentralCache的同时，要将PageCache中span映射的spanlist位置上删除
		Span* it = _spanList[k].Begin();
		_spanList[k].Erase(it);
		return it;
	}

	//   b.k页的span不存在，从PageCache的spanlist的k+1位置开始找大块span切分
	//		  i. 如果找到了将大页给切小
	for (size_t i = k + 1; i < NPAGES; ++i)
	{
			//切成k页的span返回
			//切出i-k页挂回自由链表
		if (!_spanList[i].Empty())
		{
			Span* span = _spanList[i].Begin();
			_spanList[i].Erase(span);
			Span* splitSpan = new Span;
			splitSpan->_pageId = span->_pageId + k;
			splitSpan->_n = span->_n - k;

			span->_n = k;

			_spanList[splitSpan->_n].Insert(_spanList[splitSpan->_n].Begin(), splitSpan);

			return span;
		}
	}

		// ii.0-128页span都不存在，向系统申请128页span
	Span* bigSpan = new Span;
	void* memory = SystemAllocPage(NPAGES - 1);	//向系统申请128页span
	bigSpan->_pageId = (size_t)memory >> 12;	//指针(地址)/4K计算出页号
	bigSpan->_n = NPAGES - 1;

	//申请的128页span插入到_spanList中
	_spanList[NPAGES - 1].Insert(_spanList[NPAGES - 1].Begin(), bigSpan);

	//走到这里再调用一次newspan将大页切小
	return NewSpan(k);
}