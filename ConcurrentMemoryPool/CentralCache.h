#pragma once
#include"Common.h"

class CentralCache
{
public:
	//�����Ļ����ȡһ�������Ķ����thread cache
	size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t byte_size);

	//��SpanList����page cache�л�ȡһ��span
	Span* GetOneSpan(SpanList& list, size_t byte_size);

	// ��һ�������Ķ����ͷŵ�span���
	void ReleaseListToSpans(void* start, size_t byte_size);
private:
	SpanList _spanLists[NLISTS];	//�����뷽ʽӳ�䣬��ÿһ��span������
};

static CentralCache centralCache;