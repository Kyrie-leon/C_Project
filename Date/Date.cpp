#include "Date.h"

int Date::GetMonthDay(int year, int month)
{
	static int daysArray[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int days = daysArray[month];
	if (month == 2 && 
		((year % 4 == 0 && year %100 != 0) || (year % 400 == 0)))
	{
		days = 29;
	}

	return days;
}

Date::Date(int year, int month, int day)
{
	_year = year;
	_month = month;
	_day = day;

	if (!(year >= 0 &&
		month > 0 && month < 13 &&
		day > 0 && day <= GetMonthDay(year, month)))
	{
		cout << "日期非法" << endl;
	}
}

void Date::Print()
{
	cout << _year << "-" << _month << "-" << _day << endl;
}


Date& Date::operator += (int day)
{
	if (day < 0)
	{
		return *this -= -day;
	}

	_day += day;
	// 日期是否合法
	while (_day > GetMonthDay(_year, _month))
	{
		_day -= GetMonthDay(_year, _month);
		_month++;
		if (_month == 13)
		{
			_year++;
			_month = 1;
		}
	}

	return *this;
}

// d1 + 10
Date Date::operator+(int day)
{
	// 跟+=实现，逻辑类似冗余
	/*Date ret(*this);
	ret._day += day;
	while (ret._day > GetMonthDay(ret._year, ret._month))
	{
		ret._day -= GetMonthDay(ret._year, ret._month);
		ret._month++;
		if (ret._month == 13)
		{
			ret._year++;
			ret._month = 1;
		}
	}*/

	Date ret(*this);
	ret += day; // ret.operator+=(day);

	return ret;
}

Date& Date::operator-=(int day)
{
	// ..
	return *this;
}

// d1 - 10
Date Date::operator-(int day)
{
	Date ret(*this);
	// ...
	return ret;
}

// ++d1  等价于  d1 += 1
// d1.operator++(&d1);
// 返回++之后的值
Date& Date::operator++()
{
	*this += 1;
	return *this;
}

// d1++
// d1.operator++(&d1, 0);
// 返回++之前的值
Date Date::operator++(int)
{
	Date ret(*this);
	*this += 1;

	return ret;
}

// 代码复用

// d1 == d2
bool Date::operator==(const Date& d)
{
	return _year == d._year
		&& _month == d._month
		&& _day == d._day;
}

bool Date::operator>(const Date& d)
{
	if (_year > d._year)
	{
		return true;
	}
	else if (_year == d._year)
	{
		if (_month > d._month)
		{
			return true;
		}
		else if (_month == d._month)
		{
			if (_day > d._day)
			{
				return true;
			}
		}
	}

	return false;
}

// d1 != d2
bool Date::operator!=(const Date& d)
{
	return !(*this == d);
}

// d1 >= d2
bool Date::operator>=(const Date& d)
{
	return *this > d || *this == d;
}

// d1 < d2
bool Date::operator<(const Date& d)
{
	return !(*this >= d);
}

// d1 <= d2
bool Date::operator<=(const Date& d)
{
	return !(*this > d);
}

// d1 - d2
// 1.写对
// 2.写简单
// 3.写好读，且复用性等维护性强
int Date::operator-(const Date& d)
{
	// 之间进行日期之间相减，不好操作
	Date max = *this, min = d;
	int flag = 1;
	if (*this < d)
	{
		max = d;
		min = *this;
		flag = -1;
	}

	int n = 0;
	while (min != max)
	{
		++n;
		++min;
	}

	return n*flag;
}
