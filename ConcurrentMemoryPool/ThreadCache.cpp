#include"ThreadCache.h"
#include"CentralCache.h"

//ThreadCacheΪ��ʱ�����Ļ����ȡ�ڴ�
void* ThreadCache::FetchFromCentralCache(size_t i, size_t size)
{
	//1.��ȡһ����������ʹ����������ʽ��ÿ��ȥ���Ļ���ȡҪ����Ч��̫�ͣ���˻�ȡһ������
	//NumMoveSize��MaxSizeȡ��Сֵ
	size_t batchNum = std::min(SizeClass::NumMoveSize(size), _freeLists[i].MaxSize());

	//2.�����batchNum��ȥ���Ļ����ȡһ���ڴ�
	void* start = nullptr;	
	void* end = nullptr;
	//	a. actualNum��ʾ��ʵ���صĸ���,���Ļ����ṩ���ڴ��п���С����Ҫ���ڴ�
	size_t actualNum = centralCache.FetchRangeObj(start, end, batchNum, size);
	assert(actualNum > 0);

	//		i. actualNum>1, ����һ���ڴ棬��ʣ�¹ҵ���������
	if (actualNum > 1)
	{
		//��һ������ĵ�һ��start���أ�start���漴NextObj(start)���뵽����������
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
	//1.�ȼ���λ��
	size_t i = SizeClass::Index(size);
	//2.����size�Ƿ����64K-MAXBYETS
	//	a.����64KȥPageCache�����ڴ�
	//	b.С��64K��ThreadCache������
	if (size > MAX_BYTES)
	{
		//PageCache�����ڴ�
	}
	else
	{
		//3.��ThreadCache�������������ڴ�
		//	a.��Ϊ��ֱ�ӽ��ڴ�������������г�������
		//  b.Ϊ�մ����Ļ����ȡ
		if (!_freeLists[i].Empty())
		{
			return _freeLists[i].Pop();
		}
		else
		{
			//��CentralCache��ȡ�ڴ������������ʽ
			return FetchFromCentralCache(i, size);
		}
	}

}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	//�ͷŻ�����������һ��λ�ò����ȥ�Ϳ�����
	size_t i = Index(size);
	_freeLists[i].Push(ptr);

	//�������̫���ͷŻ��յ����Ļ���
}