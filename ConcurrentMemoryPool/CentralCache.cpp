#include"CentralCache.h"
#include"PageCache.h"


//��centralCache��ȡspan
Span* CentralCache::GetOneSpan(SpanList& list, size_t size)
{
	//1.����CentralCache��spanlist���һ����ڴ��span��2�������
	//	a.����SpanList,span���ڴ�
	//  b.spanû���ڴ���pageCache�����ڴ�
	Span* it = list.Begin();
	while (it != list.End())
	{
		//i.�ҵ��˾ͷ���
		if (it->_memory)
			return it;
		//ii.û�ҵ��ͱ�����һ��
		it = it->_next;
	}

	// �ߵ����������span��û���ڴ��ˣ�b�����ֻ����pagecache
	//ͨ��NumMovePage����Ҫ����ҳ
	return pageCache.NewSpan(SizeClass::NumMovePage(size));
}

//��ThreadCache�ṩ�ڴ�
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t size)
{
	//1.�����Ӧ��λ��
	size_t i = SizeClass::Index(size);
	//2.��centralCache��ȡspan
	Span* span = GetOneSpan(_spanLists[i], size);

	// ...
	start = span->_memory;
	for (size_t i = 0; i < n; ++i)
	{

	}


}