#include "StdAfx.h"
#include "LuaInteger.h"

int LuaInteger::operator+(int a) const
{
	return m_value + a;
}

int LuaInteger::operator-(int a) const
{
	return m_value - a;
}

int LuaInteger::operator*(int a) const
{
	return (int)(m_value*a);
}

int LuaInteger::operator/(int a) const
{
	if ( a == 0 )
		return 0;

	return (int)(m_value/a);
}

bool LuaInteger::operator== (const int a) const
{
	return (m_value==a);
}

bool LuaInteger::operator<= (int a) 
{
	return (m_value<=a);
}

bool LuaInteger::operator< (int a) 
{
	return (m_value<a);
}

int LuaInteger::operator- () const
{
	return -m_value;
}

int LuaInteger::operator() () const
{
	return m_value;
}

int LuaInteger::operator() (int a) 
{
	m_value = a;
	return m_value;
}

// 	LuaInteger& LuaInteger::operator* ()
// 	{
// 		return *this;
// 	}

int operator*(LuaInteger const& lhs, LuaInteger const& rhs)
{
	int res = lhs.m_value * rhs.m_value;
	return res;
}

int operator+(int a, const LuaInteger& rhs)
{
	int res = a + rhs.m_value;
	return res;
}

int operator-(int a, const LuaInteger& rhs)
{
	int res = a - rhs.m_value;
	return res;
}

int operator*(int v, LuaInteger const& rhs)
{
	int res = rhs.m_value * v;
	return res;
}

int operator/(int v, LuaInteger const& rhs)
{
	int res = v / rhs.m_value;
	return res;
}

bool operator== (int a, LuaInteger const& rhs) 
{
	return (a == rhs.m_value);
}

bool operator<= (int a, LuaInteger const& rhs) 
{
	return (a <= rhs.m_value );
}

bool operator< (int a, LuaInteger const& rhs) 
{
	return (a < rhs.m_value);
}

bool operator== (const LuaInteger li, LuaInteger const& rhs) 
{
	return (li.m_value==rhs.m_value);
}

bool operator<= (const LuaInteger li, LuaInteger const& rhs) 
{
	return (li.m_value<=rhs.m_value);
}

bool operator< (const LuaInteger li, LuaInteger const& rhs) 
{
	return (li.m_value<rhs.m_value);
}

std::ostream& operator<<(std::ostream& os, const LuaInteger& li)
{
	os << ' ' << li.m_value << ' '; return os;
}
