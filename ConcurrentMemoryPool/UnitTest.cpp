#include"ConcurrentAlloc.h"

void TestSizeClass()
{
	cout << SizeClass::Index(1035) << endl;
	cout << SizeClass::Index(1025) << endl;
	cout << SizeClass::Index(1000) << endl;
}

void TestConcurrentAlloc()
{
	void* ptr0 = ConcurrentAlloc(5);
	void* ptr1 = ConcurrentAlloc(8);
	void* ptr2 = ConcurrentAlloc(8);
	void* ptr3 = ConcurrentAlloc(8);

	ConcurrentFree(ptr1);
	ConcurrentFree(ptr2);
	ConcurrentFree(ptr3);
}

void TestBigMemory()
{
	void* ptr1 = ConcurrentAlloc(65 * 1024);
	ConcurrentFree(ptr1);

	void* ptr2 = ConcurrentAlloc(129 * 4 * 1024);
	ConcurrentFree(ptr2);
}

//int main()
//{
//	TestSizeClass();
//	TestConcurrentAlloc();
//	TestBigMemory();
//	system("pause");
//	return 0;
//}