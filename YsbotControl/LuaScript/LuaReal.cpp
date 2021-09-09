// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "LuaReal.h"

double LuaReal::operator+(double a) const
{
	return m_value + a;
}

double LuaReal::operator-(double a) const
{
	return m_value - a;
}

double LuaReal::operator*(double a) const
{
	return (double)(m_value*a);
}

double LuaReal::operator/(double a) const
{
	if ( a == 0 )
		return 0;

	return (double)(m_value/a);
}

bool LuaReal::operator== (double a) 
{
	return (m_value==a);
}

bool LuaReal::operator<= (double a) 
{
	return (m_value<=a);
}

bool LuaReal::operator< (double a) 
{
	return (m_value<a);
}

double LuaReal::operator- () const
{
	return -m_value;
}

double LuaReal::operator() () const
{
	return m_value;
}

double LuaReal::operator() (double a) 
{
	m_value = a;
	return m_value;
}


double operator*(LuaReal const& lhs, LuaReal const& rhs)
{
	double res = lhs.m_value * rhs.m_value;
	return res;
}

double operator+(double a, const LuaReal& rhs)
{
	double res = a + rhs.m_value;
	return res;
}

double operator-(double a, const LuaReal& rhs)
{
	double res = a - rhs.m_value;
	return res;
}

double operator*(double v, LuaReal const& rhs)
{
	double res = rhs.m_value * v;
	return res;
}

double operator/(double v, LuaReal const& rhs)
{
	double res = v / rhs.m_value;
	return res;
}

bool operator== (double a, LuaReal const& rhs) 
{
	return (a == rhs.m_value);
}

bool operator<= (double a, LuaReal const& rhs) 
{
	return (a <= rhs.m_value );
}

bool operator< (double a, LuaReal const& rhs) 
{
	return (a < rhs.m_value);
}

bool operator== (const LuaReal li, LuaReal const& rhs) 
{
	return (li.m_value==rhs.m_value);
}

bool operator<= (const LuaReal li, LuaReal const& rhs) 
{
	return (li.m_value<=rhs.m_value);
}

bool operator< (const LuaReal li, LuaReal const& rhs) 
{
	return (li.m_value<rhs.m_value);
}

std::ostream& operator<<(std::ostream& os, const LuaReal& li)
{
	os << ' ' << li.m_value << ' '; return os;
}