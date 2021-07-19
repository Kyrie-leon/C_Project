#include"PageCache.h"

//��ϵͳ����kҳ�ڴ�
void* PageCache::SystemAllocPage(size_t k)
{

}

//��PageCache������һ��span����
Span* PageCache::NewSpan(size_t k)
{
	//1.����PageCache��kҳ��spanlist�Ƿ�Ϊ��
	//   a.��Ϊ��ֱ�ӷ��ؼ���
	if (!_spanList[k].Empty())
	{
		//���ظ�CentralCache��ͬʱ��Ҫ��PageCache��spanӳ���spanlistλ����ɾ��
		Span* it = _spanList[k].Begin();
		_spanList[k].Erase(it);
		return it;
	}

	//   b.kҳ��span�����ڣ���PageCache��spanlist��k+1λ�ÿ�ʼ�Ҵ��span�з�
	//		  i. ����ҵ��˽���ҳ����С
	for (size_t i = k + 1; i < NPAGES; ++i)
	{
			//�г�kҳ��span����
			//�г�i-kҳ�һ���������
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

		// ii.0-128ҳspan�������ڣ���ϵͳ����128ҳspan
	Span* bigSpan = new Span;
	void* memory = SystemAllocPage(NPAGES - 1);	//��ϵͳ����128ҳspan
	bigSpan->_pageId = (size_t)memory >> 12;	//ָ��(��ַ)/4K�����ҳ��
	bigSpan->_n = NPAGES - 1;

	//�����128ҳspan���뵽_spanList��
	_spanList[NPAGES - 1].Insert(_spanList[NPAGES - 1].Begin(), bigSpan);

	//�ߵ������ٵ���һ��newspan����ҳ��С
	return NewSpan(k);
}