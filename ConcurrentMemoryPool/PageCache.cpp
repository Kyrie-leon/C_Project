#include"PageCache.h"

PageCache PageCache::_sInst;

//��ϵͳ����kҳ�ڴ�
void* PageCache::SystemAllocPage(size_t k)
{
	return ::SystemAlloc(k);
}

//��PageCache������һ��kҳspan����
Span* PageCache::NewSpan(size_t k)
{
	std::lock_guard<std::recursive_mutex> lock(_mtx);
	//���k���ڵ���NPAGES�Ĵ���ڴ棬ֱ����ϵͳ����
	//���Խ������
	if (k >= NPAGES)
	{
		void* ptr = SystemAlloc(k);
		Span* span = new Span;
		span->_pageId = (ADDRES_INT)ptr >> PAGE_SHIFT;	//��ַӳ���PageId
		span->_n = k;
		span->_objsize = k << NPAGES;	//�������������Ĵ�С
		//����ҳ�ź͵�ַ��ӳ��
		_idSpanMap[span->_pageId] = span;

		return span;
	}

	//1.����PageCache��kҳ��spanlist�Ƿ�Ϊ��
	//   a.��Ϊ��ֱ�ӷ��ؼ���
	if (!_spanList[k].Empty())
	{
		//���ظ�CentralCache��ͬʱ��Ҫ��PageCache��spanӳ���spanlistλ����ɾ��
		return _spanList[k].PopFront();
	}

	//   b.kҳ��span�����ڣ���PageCache��spanlist��k+1λ�ÿ�ʼ�Ҵ��span�з�
	//		  i. ����ҵ��˽���ҳ����С
	for (size_t i = k + 1; i < NPAGES; ++i)
	{
			//�г�kҳ��span����
			//�г�i-kҳ�һ���������
		if (!_spanList[i].Empty())
		{
			//ͷ��
			//�ҵ���span�ȴ�spanlist��ɾ��
			//Span* span = _spanList[i].Begin();
			//_spanList[i].Erase(span);
			////span���зֳ�����span ��spiltspan
			//Span* splitSpan = new Span;
			////splitspan��page��_pageId+k��ʼ
			//splitSpan->_pageId = span->_pageId + k;
			////ҳ������-k
			//splitSpan->_n = span->_n - k;

			//span->_n = k;
			////�зֳ�����spiltspan���뵽spanlist
			//_spanList[splitSpan->_n].Insert(_spanList[splitSpan->_n].Begin(), splitSpan);

			//return span;

			//β�г�һ��kҳ��span
			Span* span = _spanList[i].PopFront();
			//��span���Ϊspan��spilt
			Span* spilt = new Span;
			spilt->_pageId = span->_pageId + span->_n - k;
			spilt->_n = k;

			for (PageID i = 0; i < k; ++i)
			{
				_idSpanMap[spilt->_pageId + i] = spilt;
			}

			span->_n -= k;
			//��ֺ��span���
			_spanList[span->_n].PushFront(span);

			return spilt;
		}
	}

		// ii.0-128ҳspan�������ڣ���ϵͳ����128ҳspan
	Span* bigSpan = new Span;
	void* memory = SystemAllocPage(NPAGES - 1);	//��ϵͳ����128ҳspan
	bigSpan->_pageId = (size_t)memory >> 12;	//ָ��(��ַ)/4K�����ҳ��
	bigSpan->_n = NPAGES - 1;
	//ҳ�ź�spanӳ���ϵ�Ľ���
	for (PageID i = 0; i < bigSpan->_n; ++i)
	{
		PageID id = bigSpan->_pageId + i;
		_idSpanMap[id] = bigSpan;
	}

	//�����128ҳspan���뵽_spanList��
	_spanList[NPAGES - 1].Insert(_spanList[NPAGES - 1].Begin(), bigSpan);

	//�ߵ������ٵ���һ��newspan����ҳ��С
	return NewSpan(k);
}

//��ȡ����Span��ӳ��
Span* PageCache::MapObjectToSpan(void* obj)
{
	//std::lock_guard<std::recursive_mutex> lock(_mtx);
	//1.//obj����12λ�������ַӳ���ҳ��
	PageID id = (ADDRES_INT)obj >> PAGE_SHIFT;
	//2.ͨ��map����ҳ�ź͵�ַ��ӳ��
	//	a.�ҵ�����span��ַ
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
	//���ڵ���NPAGES���ڴ滹��ϵͳ
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
	//���ǰ�����Ҳ�������ڴ�ϲ�������ڴ���Ƭ����

	//��ǰ�ϲ�
	while (1)
	{
		PageID preId = span->_pageId - 1;	//ǰһ��span
		//1.����ǰһ��preSpan�Ƿ����
		auto ret = _idSpanMap.find(preId);
		//	a.���������˳�ѭ��
		if (ret == _idSpanMap.end())
		{
			break;
		}
		//		i.���ڵ��ǻ���ʹ��Ҳ�˳�ѭ��
		Span* preSpan = ret->second;
		if (preSpan->_usecount != 0)
		{
			break;
		}
		//�������128ҳ���ϲ�
		if (span->_n + preSpan->_n >= NPAGES)
		{
			break;
		}
		//	b.������û��ʹ�ã�������ǰ�ϲ�
		//�ȴӶ�Ӧ��span�����н������ٺϲ�
		_spanList[preSpan->_n].Erase(preSpan);
		span->_pageId = preSpan->_pageId;
		span->_n += preSpan->_n;

		//����ҳ�͵�ַ��ӳ���ϵ
		for (PageID i = 0; i < preSpan->_n; ++i)
		{
			_idSpanMap[preSpan->_pageId + i] = span;
		}
		delete preSpan;
	}

	//���ϲ�
	while (1)
	{
		//1.�Ȳ��Һ�һ��span�ܷ����
		PageID nextId = span->_pageId + span->_n;
		auto ret = _idSpanMap.find(nextId);
		//	a.�����ھ��˳�
		if (ret == _idSpanMap.end())
		{
			break;
		}
		//		i.���ڵ���ʹ����Ҳ�˳�
		Span* nextSpan = ret->second;
		if(nextSpan->_usecount != 0)
		{
			break;
		}
		//�������128ҳ���ϲ�
		if (span->_n + nextSpan->_n >= NPAGES)
		{
			break;
		}
		//��spanlist�Ͻ�����ϲ�
		_spanList[nextSpan->_n].Erase(nextSpan);
		span->_n += nextSpan->_n;
		//����ӳ��ҳ�ź͵�ַ
		for (PageID i = 0; i < nextSpan->_n; ++i)
		{
			_idSpanMap[nextSpan->_pageId + i] = span;
		}
		delete nextSpan;
	}
	//�ϲ������span�����뵽��Ӧ��������
	_spanList[span->_n].PushFront(span);
}