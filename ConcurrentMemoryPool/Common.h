#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
#include<time.h>
#include<assert.h>
using std::cout;
using std::endl;

static const size_t MAX_BYTES = 64 * 1024;	//最大页64k
static const size_t NLISTS = 184;		//freeList范围[0, 184] 184个桶
static const size_t NPAGES = 129;


//获取下一个链表节点
inline void*& NextObj(void* obj)
{
	return *((void**)obj);
}

//1. 映射自由链表的位置
static size_t Index(size_t size)
{
	//等价于(size + 7)/8 -1  
	//例如8字节的index = (8 + 7) >> 3 = 15/8 = 1
	//这样的话就可以将[1, 8]全部映射在[8, 15]之间
	//可以不通过除或模运算就能计算出size映射在哈希桶的位置
	return ((size + (2 ^ 3 - 1)) >> 3) - 1;	
}

//管理对齐和映射的关系
class SizeClass
{
public:
	// 控制在12.5%内的内碎片浪费
	// [1,128]					8byte对齐	     freelist[0,16)
	// [129,1024]				16byte对齐		 freelist[16,72)
	// [1025,8*1024]			128byte对齐	     freelist[72,128)
	// [8*1024+1,64*1024]		1024byte对齐     freelist[128,184)

	//1. 映射自由链表的位置
	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		return ((bytes + (1 << align_shift - 1)) >> align_shift) - 1;
	}

	// 计算映射的哪一个自由链表桶
	static inline size_t Index(size_t bytes)
	{
		assert(bytes <= MAX_BYTES);

		// 每个区间有多少个链
		// 计算哪一个桶，自由链表桶划分了4个区间，每个区间个数{ 16, 56, 56, 56 }
		static int group_array[4] = { 16, 56, 56, 56 };
		//[]
		if (bytes <= 128) {
			return _Index(bytes, 3);
		}
		else if (bytes <= 1024) {
			return _Index(bytes - 128, 4) + group_array[0];
		}
		else if (bytes <= 8192) {
			return _Index(bytes - 1024, 7) + group_array[1] + group_array[0];
		}
		else if (bytes <= 65536) {
			return _Index(bytes - 8192, 10) + group_array[2] + group_array[1] + group_array[0];
		}

		assert(false);

		return -1;
	}

	//计算出一次从CentralCache获取多少个
	static size_t NumMoveSize(size_t size)
	{
		if (size == 0)
			return 0;
		//控制在[2, 512]
		//
		int num = MAX_BYTES / size;
		if (num < 2)
			num = 2;
		if (num > 512)
			num = 512;

		return num;
	}
	
	//计算一次向系统获取几个页
	//单个对象 8byte
	//...
	//单个对象64KB
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;

		//  除以4KB即右移12位算出需要多少页
		npage >>= 12;
		if (npage == 0)
			npage = 1;

		return npage;
	}

};

//挂内存对象的自由链表
class FreeList
{
public:
	//push一个范围的对象
	void PushRange(void* start, void* end, int n)
	{

	}

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
	size_t _max_size = 1;	//
};

/*
*	Span 管理一个大跨度的内存
*
*
*/

//2^32/2^12  
//2^64/2^12
typedef size_t PageID;

struct Span
{
	PageID _pageId;		//页号
	size_t _n;			//页的数量

	//span双向链表
	Span* _next = nullptr;
	Span* _prev = nullptr;	

	void* _memory = nullptr;
	size_t _usecount = 0;	//使用计数， ==0说明对象全部回收
	size_t _objsize = 0;		//切出来的单个对象大小,针对一个定长大小
};

//spanList是一个双向链表
class SpanList
{
public:
	//构造函数
	SpanList()
	{
		_head = nullptr;
		_head->_prev = _head;
		_head->_next = _head;
	}
	Span* Begin()
	{
		return _head->_next;
	}

	Span* End()
	{
		return _head;
	}
	
	//插入
	void Insert(Span* cur, Span* newspan)
	{
		Span* prev = cur->_prev;
		prev->_next = newspan;
		newspan->_prev = prev;
		newspan->_next = cur;
		cur->_prev = newspan;
	}

	//删除
	void Erase(Span* cur)
	{
		assert(_head != cur);	//不为空

		Span* prev = cur->_prev;
		Span* next = cur->_next;

		prev->_next = next;
		next->_prev = prev;
	}

	bool Empty()
	{
		return _head->_prev == _head->_next;
	}
private:
	Span* _head;	//头节点
};