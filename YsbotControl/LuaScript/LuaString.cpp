#include "StdAfx.h"
#include "LuaString.h"

std::string LuaString::operator+(std::string a) 
{
	return m_value + a;
}

bool LuaString::operator== (std::string a) 
{
	return (m_value==a);
}

std::string LuaString::operator() () const
{
	return m_value;
}

std::string LuaString::operator() (std::string a) 
{
	m_value = a;
	return m_value;
}

std::string operator+(std::string a, LuaString& rhs)
{
	std::string res = a + rhs.m_value;
	return res;
}

bool operator== (std::string a, LuaString const& rhs) 
{
	return (a == rhs.m_value);
}

bool operator== (const LuaString li, LuaString const& rhs) 
{
	return (li.m_value==rhs.m_value);
}


std::ostream& operator<<(std::ostream& os, const LuaString& li)
{
	os << ' ' << li.m_value << ' '; return os;
}