#pragma once
#include"Common.h"

template<class T>
class ObjectPool
{
	//1.构造函数
	ObjectPool()
	{}
	//2.析构函数
	~ObjectPool()
	{

	}

	//获取下一个节点
	void*& NextObj(void* obj)
	{
		return (*(void**)obj);
	}

	//3.New()申请空间
	T* New()
	{	
		T* obj = nullptr;
		//freeList不为空
		if (_freeList)
		{
			obj = (T *)_freeList;	//frerList地址传给obj
			_freeList = NextObj(_freeList);		//freeList指向下一个节点
		}
		else
		{
			//_memory为空代表没有空间可以分配，需要向系统申请分配空间
			if (_memory == nullptr)
			{
				_leftsize = 1024 * 100;
				_memory = (void *)malloc(_leftsize);
				//申请失败抛异常
				if (_memory == nullptr)
				{
					throw std::bad_alloc();
				}
			}
			obj = _memory;
			_memory += sizeof(T);
			_leftsize -= sizeof(T);
		}

		new(obj)T;
		return obj;
	}
	//4.Delete()释放空间
	void Delete(T* obj)
	{
		obj->~T();
		NextObj(obj) = _freeList;	//头插入freelist
	}
private:
	void* _memory = nullptr;	//内存地址
	size_t _leftsize = 0;	//内存剩余空间
	void* _freeList = nullptr;	//
};

struct TreeNode
{
	int _val;
	TreeNode* _left;
	TreeNode* _right;

	TreeNode()
		:_val(0)
		, _left(nullptr)
		, _right(nullptr)
	{}
};

void TestObjectPool()
{
	/*ObjectPool<TreeNode> tnPool;
	std::vector<TreeNode*> v;
	for (size_t i = 0; i < 100; ++i)
	{
	TreeNode* node = tnPool.New();
	cout << node << endl;
	v.push_back(node);
	}

	for (auto e : v)
	{
	tnPool.Delete(e);
	}*/

	/*ObjectPool<TreeNode> tnPool;
	TreeNode* node1 = tnPool.New();
	TreeNode* node2 = tnPool.New();
	TreeNode* node3 = tnPool.New();
	TreeNode* node4 = tnPool.New();
	cout << node1 << endl;
	cout << node2 << endl;
	cout << node3 << endl;
	cout << node4 << endl;
	tnPool.Delete(node1);
	tnPool.Delete(node4);


	TreeNode* node10 = tnPool.New();
	cout << node10 << endl;

	TreeNode* node11 = tnPool.New();
	cout << node11 << endl;*/

	size_t begin1 = clock();
	std::vector<TreeNode*> v1;
	for (int i = 0; i < 100000; ++i)
	{
		v1.push_back(new TreeNode);
	}
	for (int i = 0; i < 100000; ++i)
	{
		delete v1[i];
	}
	v1.clear();

	for (int i = 0; i < 100000; ++i)
	{
		v1.push_back(new TreeNode);
	}

	for (int i = 0; i < 100000; ++i)
	{
		delete v1[i];
	}
	v1.clear();
	size_t end1 = clock();


	ObjectPool<TreeNode> tnPool;
	size_t begin2 = clock();
	std::vector<TreeNode*> v2;
	for (int i = 0; i < 100000; ++i)
	{
		v2.push_back(tnPool.New());
	}
	for (int i = 0; i < 100000; ++i)
	{
		tnPool.Delete(v2[i]);
	}
	v2.clear();

	for (int i = 0; i < 100000; ++i)
	{
		v2.push_back(tnPool.New());
	}
	for (int i = 0; i < 100000; ++i)
	{
		tnPool.Delete(v2[i]);
	}
	v2.clear();

	size_t end2 = clock();

	cout << end1 - begin1 << endl;
	cout << end2 - begin2 << endl;


	ObjectPool<char> chPool;

}