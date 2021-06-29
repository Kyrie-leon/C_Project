#pragma once
#include<iostream>
#include<vector>
#include<time.h>
using std::cout;
using std::endl;

static const size_t MAX_BYTES = 64 * 1024;	//���ҳ64k
static const size_t NFREELISTS = 184;		//freeList��Χ

//��ȡ��һ������ڵ�
inline void*& NextObj(void* obj)
{
	return *((void**)obj);
}

//ӳ�����������λ��,static?
static size_t Index(size_t size)
{
	return ((size + (2 ^ 3 - 1)) >> 3) - 1;	//�ȼ���(size + 7)/8 -1
}

//��������ӳ��Ĺ�ϵ
class SizeClass
{

};

//���ڴ�������������
class FreeList
{
public:
	//�ж������Ƿ�Ϊ��
	bool Empty() const
	{
		return _head == nullptr;	//ͷ���Ϊ��������Ϊ��
	}

	//ͷ��
	void Push(void* obj)
	{
		NextObj(obj) = _head;	//obj->next
		_head = obj;	//_head->obj->next
	}

	//ͷɾ
	void* Pop()
	{
		void* obj = _head;
		_head = NextObj(_head);

		return obj;
	}

	size_t MaxSize()
	{
		return _max_size;
	}

	void SetMaxSize(size_t n)
	{
		_max_size = n;
	}
private:
	void* _head = nullptr;	//ͷ�ڵ�
	size_t _max_size = 1;	//??
};