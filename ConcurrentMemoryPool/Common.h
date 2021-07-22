#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
#include<time.h>
#include<assert.h>
using std::cout;
using std::endl;

static const size_t MAX_BYTES = 64 * 1024;	//���ҳ64k
static const size_t NLISTS = 184;		//freeList��Χ[0, 184] 184��Ͱ
static const size_t NPAGES = 129;
static const size_t PAGE_SHIFT = 12;

#ifdef  _WIN32
#include<windows.h>
#else
//
#endif

//��ȡ��һ������ڵ�
inline void*& NextObj(void* obj)
{
	return *((void**)obj);
}

//1. ӳ�����������λ��
static size_t Index(size_t size)
{
	//�ȼ���(size + 7)/8 -1  
	//����8�ֽڵ�index = (8 + 7) >> 3 = 15/8 = 1
	//�����Ļ��Ϳ��Խ�[1, 8]ȫ��ӳ����[8, 15]֮��
	//���Բ�ͨ������ģ������ܼ����sizeӳ���ڹ�ϣͰ��λ��
	return ((size + (2 ^ 3 - 1)) >> 3) - 1;	
}

//��������ӳ��Ĺ�ϵ
class SizeClass
{
public:
	// ������12.5%�ڵ�����Ƭ�˷�
	// [1,128]					8byte����	     freelist[0,16)
	// [129,1024]				16byte����		 freelist[16,72)
	// [1025,8*1024]			128byte����	     freelist[72,128)
	// [8*1024+1,64*1024]		1024byte����     freelist[128,184)

	//1. ӳ�����������λ��
	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		return ((bytes + (1 << align_shift - 1)) >> align_shift) - 1;
	}

	// ����ӳ�����һ����������Ͱ
	static inline size_t Index(size_t bytes)
	{
		assert(bytes <= MAX_BYTES);

		// ÿ�������ж��ٸ���
		// ������һ��Ͱ����������Ͱ������4�����䣬ÿ���������{ 16, 56, 56, 56 }
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

	//�����һ�δ�CentralCache��ȡ���ٸ�
	static size_t NumMoveSize(size_t size)
	{
		if (size == 0)
			return 0;
		//������[2, 512]��һ�������ƶ����ٸ����������ֵ
		//��������ԽСnumԽ�󣬵�������Խ��numԽС�����������̫���˷�
		int num = MAX_BYTES / size;
		if (num < 2)
			num = 2;
		if (num > 512)
			num = 512;

		return num;
	}
	
	//����һ����ϵͳ��ȡ����ҳ
	//�������� 8byte
	//...
	//��������64KB
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;

		//  ����4KB������12λ�����Ҫ����ҳ
		npage >>= 12;
		if (npage == 0)
			npage = 1;

		return npage;
	}

};

//���ڴ�������������
class FreeList
{
public:
	//pushһ����Χ�Ķ���,��һ���ڴ�����ҽ���
	void PushRange(void* start, void* end, int n)
	{
		NextObj(end) = _head;
		_head = start;
		_size += n;		//����һ���ڴ棬size+n
	}

	void PopRange(void*& start, void*& end, int n)
	{

	}

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
		++_size;
	}

	//ͷɾ
	void* Pop()
	{
		void* obj = _head;
		_head = NextObj(_head);
		--_size;

		return obj;
	}

	size_t Size()
	{
		return _size;
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
	size_t _max_size = 1;
	size_t _size = 0;		//���ڻ���
};

/*
*	Span ����һ�����ȵ��ڴ�
*	��ҳΪ��λ�Ĵ���ڴ�
*
*/

//2^32/2^12  
//2^64/2^12
typedef size_t PageID;

struct Span
{
	PageID _pageId;		//ҳ�ţ�����ϲ�
	size_t _n;			//ҳ��������span�ڴ�4nK

	//span˫������
	Span* _next = nullptr;
	Span* _prev = nullptr;	

	void* _list = nullptr;	//����ڴ���С�����������������
	size_t _usecount = 0;	//ʹ�ü����� ==0˵������ȫ������
	size_t _objsize = 0;	//�г����ĵ��������С,���һ��������С
};

//spanList��һ��˫������
class SpanList
{
public:
	//���캯��
	SpanList()
	{
		_head = new Span;
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
	
	//����
	void Insert(Span* cur, Span* newspan)
	{
		Span* prev = cur->_prev;
		prev->_next = newspan;
		newspan->_prev = prev;
		newspan->_next = cur;
		cur->_prev = newspan;
	}

	//ɾ��
	void Erase(Span* cur)
	{
		assert(_head != cur);	//��Ϊ��

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
	Span* _head;	//ͷ�ڵ�
};

inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kpage*(1 << PAGE_SHIFT),
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	// brk mmap��
#endif
	if (ptr == nullptr)
		throw std::bad_alloc();
	return ptr;
}

inline static void SystemFree(void* ptr)
{
#ifdef _WIN32
	VirtualFree(ptr, 0, MEM_RELEASE);
#else
	// sbrk unmmap��
#endif
}