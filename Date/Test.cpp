#include<iostream>
using namespace std;

// xxx.h
//class Stack
//{
//public:
//	void Init(int capacity = 4);
//	void Destory();
//	void Push(int x);
//	void Pop();
//	// ...
//private:
//	int* _a;
//	int _size;
//	int _capacity;
//};
//
//// xxx.cpp
//void Stack::Init(int capacity)
//{
//	_a = (int*)malloc(sizeof(int) * capacity);
//	_size = 0;
//	_capacity = capacity;
//}
//
//void Stack::Push(int x)
//{
//
//}
//
//int main()
//{
//	Stack st;
//	//st.Init();
//	st.Init(100);
//	st.Push(1);
//	st.Push(2);
//	st.Push(3);
//
//	st.Destory();
//
//	return 0;
//}

//class A
//{
//public:
//	void PrintA()
//	{
//		cout << _a << endl;
//	}
////private:
//	char _a;
//};
//
//// 类中仅有成员函数
//class A2 {
//public:
//	void f2() {}
//};
//
//// 类中什么都没有---空类
//class A3
//{};
//
//int main()
//{
//
//	A a;
//	cout << sizeof(a) << endl;
//	cout << sizeof(A) << endl;
//	A a1;
//	A a2;
//	A a3;
//	A a4;
//	// 成员变量对于每个对象是独立的，成员函数对于每个对象是公共
//	// 类对象的大小，只计算成员变量+内存对齐计算
//	a1.PrintA(); // 这里函数的地址不需要到对象中去找
//	a1._a;		// 这里是到对象中去找_a
//	a2._a;
//
//	// 空类给1个byte是为了占位，表示实例化出的对象存在，不存储数据
//	cout << sizeof(A2) << endl;
//	cout << sizeof(A3) << endl;
//	A2 aa2;
//	A2 aaa2;
//	cout << &aa2 << endl;
//	cout << &aaa2 << endl;
//
//	return 0;
//}

//class Date
//{
//public:
//	void Init(int year, int month, int day)
//	{
//		cout << "this:" << this << endl;
//		this->_year = year;
//		this->_month = month;
//		this->_day = day;
//	}
//
//	void Print()
//	{
//		cout << _year << "-" << _month << "-" << _day << endl;
//	}
//
//private:
//	int _year;
//	int _month;
//	int _day;
//};
//
//int main()
//{
//	Date d1;
//	d1.Init(2021, 2, 2);
//	d1.Print();
//
//	Date d2;
//	d2.Init(2021, 2, 3);
//	d2.Print();
//
//	return 0;
//}

// 1.下面程序能编译通过吗？
// 2.下面程序会崩溃吗？在哪里崩溃
//class A
//{
//public:
//	void PrintA()
//	{
//		cout << _a << endl;
//	}
//
//	void Show()
//	{
//		cout << "Show()" << endl;
//	}
//private:
//	int _a;
//};
//
//int main()
//{
//	A* p = nullptr;
//	//p->PrintA();
//	p->Show();
//}

//class Date
//{
//public:
//
//	//Date(int year, int month, int day)
//	//{
//	//	cout << "Date(int year, int month, int day)" << endl;
//	//	_year = year;
//	//	_month = month;
//	//	_day = day;
//	//}
//
//	Date()
//	{
//		cout << "Date()" << endl;
//
//		_year = 1900;
//		_month = 1;
//		_day = 1;
//	}
//
//	Date(int year = 1900, int month = 1, int day = 1)
//	{
//		cout << "Date(int year, int month, int day)" << endl;
//		_year = year;
//		_month = month;
//		_day = day;
//	}
//
//	//void Init(int year, int month, int day)
//	//{
//	//	cout << "this:" << this << endl;
//	//	this->_year = year;
//	//	this->_month = month;
//	//	this->_day = day;
//	//}
//
//	void Print()
//	{
//		cout << _year << "-" << _month << "-" << _day << endl;
//	}
//
//private:
//	int _year;
//	int _month;
//	int _day;
//};
//
//class Stack
//{
//public:
//	// 构造函数
//	//Stack()
//	//{
//	//	_a = (int*)malloc(sizeof(int)*4);
//	//	_size = 0;
//	//	_capacity = 4;
//	//}
//
//	//Stack(int capacity)
//	//{
//	//	_a = (int*)malloc(sizeof(int) * capacity);
//	//	_size = 0;
//	//	_capacity = capacity;
//	//}
//
//	Stack(int capacity = 4)
//	{
//		_a = (int*)malloc(sizeof(int) * capacity);
//		_size = 0;
//		_capacity = capacity;
//	}
//
//	//Stack(int* a, int size, int capacity);  不实用
//private:
//	int* _a;
//	int _size;
//	int _capacity;
//};
//
//int main()
//{
//	// 想要是，对象定义(构造)出来就初始化了
//	// 我们没写，编译器默认生成了构造函数，但是这里d1里面的值还是随机值
//	// 说明默认的构造在这里不行，需要我们自己写
//	Date d1(2021, 2, 2);
//	d1.Print();
//
//	Date d2;
//	d2.Print();
//
//	Stack st1;
//	Stack st2(10);
//
//	return 0;
//}

//class Time
//{
//public:
//	Time(int hour)
//	{
//		cout << "Time()" << endl;
//		_hour = hour;
//		_minute = 0;
//		_second = 0;
//	}
//private:
//	int _hour;
//	int _minute;
//	int _second;
//};
//
//class Date
//{
//private:
//	// 基本类型(内置类型)
//	int _year;
//	int _month;
//	int _day;
//
//	// 自定义类型
//	Time _t;
//};
//
//int main()
//{
//	Date d;
//
//
//	return 0;
//}

//class String
//{
//public:
//	String(const char* str = "jack")
//	{
//		_str = (char*)malloc(strlen(str) + 1);
//		strcpy(_str, str);
//	}
//
//	~String()
//	{
//		cout << "~String()" << endl;
//		free(_str);
//	}
//private:
//	char* _str;
//};
//
//
//class Date
//{
//public:
//	Date(int year = 1900, int month = 1, int day = 1)
//	{
//		cout << "Date(int year, int month, int day)" << endl;
//		_year = year;
//		_month = month;
//		_day = day;
//	}
//
//	//~Date()
//	//{
//	//	// 清理工作，Date类中没有需要清理的资源
//	//	// 所以严格来说Date不需要写析构函数，因为我们不写，编译器默认生成就可以用
//	//	//cout << "~Date()" << endl;
//	//}
//
//	void Print()
//	{
//		cout << _year << "-" << _month << "-" << _day << endl;
//	}
//
//private:
//	// 默认析构函数，针对内置类型不处理
//	int _year;  
//	int _month;
//	int _day;
//
//	// 默认析构函数，针对自定义类型要处理的，调用他的析构函数
//	String _str;
//};
//
//class Stack
//{
//public:
//	Stack(int capacity = 4)
//	{
//		cout << "Stack(int capacity = 4)" << endl;
//
//		_a = (int*)malloc(sizeof(int) * capacity);
//		_size = 0;
//		_capacity = capacity;
//	}
//
//	// 像Date，没有资源需要清理，所以不需要自己实现析构函数
//	// 像Stack，一定要清理释放资源，所以一定要自己实现析构函数
//	~Stack()
//	{
//		cout << "~Stack()" << endl;
//
//		free(_a);
//		_a = nullptr;
//		_size = _capacity = 0;
//	}
//private:
//	int* _a;
//	int _size;
//	int _capacity;
//};
//
//int main()
//{
//	Date d1;
//
//	Stack st;
//
//	return 0;
//}

#include "Date.h"

int main()
{
	Date d1(2021, 2, 3);
	d1.Print();

	Date ret = d1 + 100;
	ret.Print();

	d1 += 100;
	d1.Print();

	ret = d1 + -100;
	ret.Print();

	d1++;
	++d1;

	//Date d2(2020, 2, 29);
	//d2.Print();

	//Date d3(2021, 13, 0);
	//d3.Print();

	Date d3(2021, 2, 3);
	Date d4(2021, 8, 1);

	cout << d3 - d4 << endl;
	cout << d4 - d3 << endl;


	return 0;
}