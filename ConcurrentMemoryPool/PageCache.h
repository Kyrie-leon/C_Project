#pragma once
#include"Common.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_sInst;
	}
	//��ϵͳ����kҳ�ڴ�ҵ���������
	void* SystemAllocPage(size_t k);
	
	//����һ���µ�span
	Span* NewSpan(size_t k);

	// ��ȡ�Ӷ���span��ӳ��
	Span* MapObjectToSpan(void* obj);
	// �ͷſ���span�ص�Pagecache�����ϲ����ڵ�span
	void ReleaseSpanToPageCahce(Span* span);


private:
	SpanList _spanList[NPAGES];	//��ҳ��ӳ��
	std::unordered_map<PageID, Span*> _idSpanMap;	//ҳ�ź��ڴ��ַ��ӳ��

	std::recursive_mutex _mtx;

private:
	PageCache()
	{}

	PageCache(const PageCache&) = delete;
	//����
	static PageCache _sInst;
};