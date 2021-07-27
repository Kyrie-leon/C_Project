#include"ThreadCache.h"
#include"CentralCache.h"

//ThreadCache为空时从中心缓存获取内存
void* ThreadCache::FetchFromCentralCache(size_t i, size_t size)
{
	//1.获取一批对象，数量使用慢启动方式，每次去中心缓存取要加锁效率太低，因此获取一批对象
	//NumMoveSize和MaxSize取最小值，NumMoveSize是上限值
	size_t batchNum = min(SizeClass::NumMoveSize(size), _freeLists[i].MaxSize());

	//2.计算出batchNum后去中心缓存获取一批内存
	void* start = nullptr;	
	void* end = nullptr;
	//	a. actualNum表示真实返回的个数,中心缓存提供的内存有可能小于需要的内存
	//		注意：这里将size对齐再传过去
	size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start, end, batchNum, SizeClass::RoundUp(size));
	assert(actualNum > 0);

	//		i. actualNum>1, 返回一个内存，将剩下挂到自由链表
	//						如果一次申请多个，剩下的挂起来不需要到中心缓存申请内存，减少锁竞争，提高效率
	if (actualNum > 1)
	{
		//将一批对象的第一个start返回，start后面即NextObj(start)插入到自由链表中
		_freeLists[i].PushRange(NextObj(start), end, actualNum - 1);
	}

	if (_freeLists[i].MaxSize() == batchNum)
	{
		_freeLists[i].SetMaxSize(_freeLists[i].MaxSize() + 1);
	}

	return start;
}

void* ThreadCache::Allocate(size_t size)
{
	//1.先计算位置
	size_t i = SizeClass::Index(size);
	//2.计算size是否大于64K-MAXBYETS
	//	a.大于64K去PageCache申请内存
	//	b.小于64K从ThreadCache中申请
		//3.向ThreadCache自由链表申请内存
		//	a.不为空直接将内存从自由链表中切出来返回
		//  b.为空从中心缓存获取
	if (!_freeLists[i].Empty())
	{
		return _freeLists[i].Pop();
	}
	else
	{
		//从CentralCache获取内存采用慢启动方式
		return FetchFromCentralCache(i, size);
	}

}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	//释放回来后算在哪一个位置插入进去就可以了
	size_t i = Index(size);
	_freeLists[i].Push(ptr);

	//如果链表太长释放回收到中心缓存
	if (_freeLists[i].Size() >= _freeLists[i].MaxSize())
	{
		ListTooLong(_freeLists[i], size);
	}
}

//链表太长回收到中心缓存
void ThreadCache::ListTooLong(FreeList& list, size_t size)
{
	//计算回收的大小
	size_t batchNum = list.MaxSize();
	void* start = nullptr;
	void* end = nullptr;
	//先从freeList链表上取下来获取到头尾结点的地址
	list.PopRange(start, end, batchNum);
	//回收到中心缓存
	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}

