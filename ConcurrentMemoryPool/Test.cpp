#include"ConcurrentAlloc.h"

void TestConcurrentAlloc()
{
	void* ptr1 = ConcurrentAlloc(8);
	void* ptr2 = ConcurrentAlloc(16);

}

int main()
{
	TestConcurrentAlloc();
	return 0;
}