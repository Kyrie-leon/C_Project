#include"PageCache.h"

PageCache PageCache::_sInst;

//向系统申请k页内存
void* PageCache::SystemAllocPage(size_t k)
{
	return ::SystemAlloc(k);
}

//从PageCache中申请一个k页span出来
Span* PageCache::NewSpan(size_t k)
{
	std::lock_guard<std::recursive_mutex> lock(_mtx);
	//针对k大于等于NPAGES的大块内存，直接向系统申请
	//解决越界问题
	if (k >= NPAGES)
	{
		void* ptr = SystemAlloc(k);
		Span* span = new Span;
		span->_pageId = (ADDRES_INT)ptr >> PAGE_SHIFT;	//地址映射出PageId
		span->_n = k;
		span->_objsize = k << NPAGES;	//计算出单个对象的大小
		//建立页号和地址的映射
		_idSpanMap[span->_pageId] = span;

		return span;
	}

	//1.先找PageCache中k页的spanlist是否为空
	//   a.不为空直接返回即可
	if (!_spanList[k].Empty())
	{
		//返回给CentralCache的同时，要将PageCache中span映射的spanlist位置上删除
		return _spanList[k].PopFront();
	}

	//   b.k页的span不存在，从PageCache的spanlist的k+1位置开始找大块span切分
	//		  i. 如果找到了将大页给切小
	for (size_t i = k + 1; i < NPAGES; ++i)
	{
			//切成k页的span返回
			//切出i-k页挂回自由链表
		if (!_spanList[i].Empty())
		{
			//头切
			//找到的span先从spanlist中删除
			//Span* span = _spanList[i].Begin();
			//_spanList[i].Erase(span);
			////span被切分出两个span 和spiltspan
			//Span* splitSpan = new Span;
			////splitspan的page从_pageId+k开始
			//splitSpan->_pageId = span->_pageId + k;
			////页的数量-k
			//splitSpan->_n = span->_n - k;

			//span->_n = k;
			////切分出来的spiltspan插入到spanlist
			//_spanList[splitSpan->_n].Insert(_spanList[splitSpan->_n].Begin(), splitSpan);

			//return span;

			//尾切出一个k页的span
			Span* span = _spanList[i].PopFront();
			//将span拆分为span和spilt
			Span* spilt = new Span;
			spilt->_pageId = span->_pageId + span->_n - k;
			spilt->_n = k;

			for (PageID i = 0; i < k; ++i)
			{
				_idSpanMap[spilt->_pageId + i] = spilt;
			}

			span->_n -= k;
			//拆分后的span插回
			_spanList[span->_n].PushFront(span);

			return spilt;
		}
	}

		// ii.0-128页span都不存在，向系统申请128页span
	Span* bigSpan = new Span;
	void* memory = SystemAllocPage(NPAGES - 1);	//向系统申请128页span
	bigSpan->_pageId = (size_t)memory >> 12;	//指针(地址)/4K计算出页号
	bigSpan->_n = NPAGES - 1;
	//页号和span映射关系的建立
	for (PageID i = 0; i < bigSpan->_n; ++i)
	{
		PageID id = bigSpan->_pageId + i;
		_idSpanMap[id] = bigSpan;
	}

	//申请的128页span插入到_spanList中
	_spanList[NPAGES - 1].Insert(_spanList[NPAGES - 1].Begin(), bigSpan);

	//走到这里再调用一次newspan将大页切小
	return NewSpan(k);
}

//获取对象到Span的映射
Span* PageCache::MapObjectToSpan(void* obj)
{
	//std::lock_guard<std::recursive_mutex> lock(_mtx);
	//1.//obj右移12位计算出地址映射的页号
	PageID id = (ADDRES_INT)obj >> PAGE_SHIFT;
	//2.通过map查找页号和地址的映射
	//	a.找到返回span地址
	auto ret = _idSpanMap.find(id);
	if (ret != _idSpanMap.end())
	{
		return ret->second;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}

void PageCache::ReleaseSpanToPageCahce(Span* span)
{
	//大于等于NPAGES的内存还给系统
	if (span->_n >= NPAGES)
	{
		//std::lock_guard<std::mutex> lock(_map_mtx);
		_idSpanMap.erase(span->_pageId);
		void* ptr = (void*)(span->_pageId << PAGE_SHIFT);
		SystemFree(ptr);
		delete span;
		return;
	}
	std::lock_guard<std::recursive_mutex> lock(_mtx);
	//检查前后空闲也，进行内存合并，解决内存碎片问题

	//向前合并
	while (1)
	{
		PageID preId = span->_pageId - 1;	//前一个span
		//1.查找前一个preSpan是否存在
		auto ret = _idSpanMap.find(preId);
		//	a.不存在则退出循环
		if (ret == _idSpanMap.end())
		{
			break;
		}
		//		i.存在但是还在使用也退出循环
		Span* preSpan = ret->second;
		if (preSpan->_usecount != 0)
		{
			break;
		}
		//如果超过128页不合并
		if (span->_n + preSpan->_n >= NPAGES)
		{
			break;
		}
		//	b.存在且没有使用，进行向前合并
		//先从对应的span链表中解下来再合并
		_spanList[preSpan->_n].Erase(preSpan);
		span->_pageId = preSpan->_pageId;
		span->_n += preSpan->_n;

		//更新页和地址的映射关系
		for (PageID i = 0; i < preSpan->_n; ++i)
		{
			_idSpanMap[preSpan->_pageId + i] = span;
		}
		delete preSpan;
	}

	//向后合并
	while (1)
	{
		//1.先查找后一个span受否存在
		PageID nextId = span->_pageId + span->_n;
		auto ret = _idSpanMap.find(nextId);
		//	a.不存在就退出
		if (ret == _idSpanMap.end())
		{
			break;
		}
		//		i.存在但是使用中也退出
		Span* nextSpan = ret->second;
		if(nextSpan->_usecount != 0)
		{
			break;
		}
		//如果超过128页不合并
		if (span->_n + nextSpan->_n >= NPAGES)
		{
			break;
		}
		//从spanlist上解挂向后合并
		_spanList[nextSpan->_n].Erase(nextSpan);
		span->_n += nextSpan->_n;
		//重新映射页号和地址
		for (PageID i = 0; i < nextSpan->_n; ++i)
		{
			_idSpanMap[nextSpan->_pageId + i] = span;
		}
		delete nextSpan;
	}
	//合并出大的span，插入到对应的链表中
	_spanList[span->_n].PushFront(span);
}