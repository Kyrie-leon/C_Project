#pragma once
#include<iostream>
#include<vector>
#include<time.h>
using std::cout;
using std::endl;

static const size_t MAX_BYTES = 64 * 1024;	//最大页64k
static const size_t NFREELISTS = 184;		//freeList范围

//获取下一个链表节点
inline void*& NextObj(void* obj)
{
	return *((void**)obj);
}

//映射自由链表的位置,static?
static size_t Index(size_t size)
{
	return ((size + (2 ^ 3 - 1)) >> 3) - 1;	//等价于(size + 7)/8 -1
}

//管理对其和映射的关系
class SizeClass
{

};

//挂内存对象的自由链表
class FreeList
{
public:
	//判断链表是否为空
	bool Empty() const
	{
		return _head == nullptr;	//头结点为空则链表为空
	}

	//头插
	void Push(void* obj)
	{
		NextObj(obj) = _head;	//obj->next
		_head = obj;	//_head->obj->next
	}

	//头删
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
	void* _head = nullptr;	//头节点
	size_t _max_size = 1;	//??
};