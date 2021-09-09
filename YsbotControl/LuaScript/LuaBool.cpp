#include "StdAfx.h" 
#include "LuaBool.h"

bool LuaBool::operator== (bool a) 
{
	return (m_value==(bool)a);
}

bool LuaBool::operator() () const
{
	return m_value;
}

bool LuaBool::operator() (bool a) 
{
	m_value = a;
	return m_value;
}

bool operator== (bool a, LuaBool const& rhs) 
{
	return ((bool)a == rhs.m_value);
}

bool operator== (const LuaBool li, LuaBool const& rhs) 
{
	return (li.m_value==rhs.m_value);
}

std::ostream& operator<<(std::ostream& os, const LuaBool& li)
{
	os << ' ' << (li.m_value ? "true":"false") << ' '; return os;
}