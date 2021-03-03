#pragma once
#include <iostream>
using namespace std;

class Date
{
public:
	int GetMonthDay(int year, int month);
	void Print();

	Date(int year = 1900, int month = 1, int day = 1);
	// 拷贝构造函数
	// d2(d1)
	//Date(const Date& d);

	// 赋值运算符重载
	// d2 = d3 -> d2.operator=(&d2, d3)
	//Date& operator=(const Date& d);

	// 析构函数
	//~Date();

	// d1 += 10
	Date& operator+=(int day);
	// d1 + 10
	Date operator+(int day);

	// d1 -= 10
	Date& operator-=(int day);
	// d1 - 10
	Date operator-(int day);

	// ++d1
	// d1.operator++(&d1);
	Date& operator++();

	// 为了跟前置++区别，迫不得已，这里给后置++，
	// 加了一个用以区别的形参，这个参数不使用,仅仅是为了跟前置++构成函数重载区分开来
	// d1++
	// d1.operator++(&d1, 0);
	Date operator++(int);

	Date& operator--();
	Date operator--(int);

	// ==运算符重载
	bool operator==(const Date& d);

	// !=运算符重载
	bool operator != (const Date& d);

	// >运算符重载
	bool operator>(const Date& d);

	// <运算符重载
	bool operator < (const Date& d);

	// >=运算符重载
	bool operator >= (const Date& d);

	// <=运算符重载
	bool operator <= (const Date& d);

	// d1 - d2
	int operator-(const Date& d);
private:
	int _year;
	int _month;
	int _day;
};