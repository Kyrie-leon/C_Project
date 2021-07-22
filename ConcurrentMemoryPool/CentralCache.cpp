#include"CentralCache.h"
#include"PageCache.h"

CentralCache CentralCache::_sInst;

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
		if (it->_list)
			return it;
		//ii.û�ҵ��ͱ�����һ��
		it = it->_next;
	}

	// �ߵ����������span��û���ڴ��ˣ�b�����ֻ����pagecache
	//ͨ��NumMovePage����Ҫ����ҳ
	return PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(size));
}

//��ThreadCache�ṩ�ڴ�
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t size)
{
	//1.�����Ӧ��λ��
	size_t index = SizeClass::Index(size);
	//2.��centralCache��ȡspan
	Span* span = GetOneSpan(_spanLists[index], size);

	// �з�span,�п���spanֻ��list��������start��end��ָ��span->_list
	start = span->_list;
	void* prev = start;
	void* cur = start;
	size_t i = 1;	//end��n-1�����i=1
	//end������n-1���г�n���ڴ�
	//ע�⣺���span�����ṩn���ڴ棬�ж��ٸ�������˼�һ��end��Ϊ�յ�����
	for (;i <= n && cur != nullptr; ++i)
	{
		prev = cur;
		cur = NextObj(cur);
		span->_usecount++;
	}

	//_listָ���зֺ���ڴ���׸�Ԫ��,�ֳ�ȥ��end�ÿ�
	span->_list = cur;
	end = prev;
	NextObj(prev) = nullptr;

	//��ʵ���صĸ���i-1
	return i-1;
}

//list����span
void CentralCache::ReleaseListToSpans(void* start, size_t byte_size)
{
	//1.����ӳ���±�
	size_t i = SizeClass::Index(byte_size);

	while (start)
	{
		//������һ�����
		void* next = NextObj(start);
		//��start�����ĸ�span
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);
		//ͷ��
		NextObj(start) = span->_list;
		span->_list = start;
		//span��usecount��1
		span->_usecount--;
		//��usecountΪ0ʱ˵��span�г�ȡ���Ĵ���ڴ涼��������
		if (span->_usecount == 0)
		{
			//�ȴ�span������
			_spanLists[i].Erase(span);
			span->_list = nullptr;
			//����pagecache
			PageCache::GetInstance()->ReleaseSpanToPageCahce(span);
		}

		start = next;
	}
}