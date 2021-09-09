#ifndef _lua_integer_h
#define _lua_integer_h
#include <ostream>

class LuaInteger
{
public:
	int m_value;
	LuaInteger() : m_value(0) {}
	LuaInteger(int val) : m_value(val) {}
	LuaInteger(const LuaInteger& lt) : m_value(lt.m_value) {}

	int operator+(int a) const;

	int operator-(int a) const;
	
	int operator*(int a) const;
 
	int operator/(int a) const;

	bool operator== (const int a) const;

	bool operator<= (int a); 

	bool operator< (int a); 

	int operator- () const;

	int operator() () const;
	int operator() (int a); 

// 	LuaInteger& operator* ()
// 	{
// 		return *this;
// 	}
	friend int operator*(LuaInteger const& lhs, LuaInteger const& rhs);

	friend int operator+(int a, const LuaInteger& rhs);

	friend int operator-(int a, const LuaInteger& rhs);

	friend int operator*(int v, LuaInteger const& rhs);

	friend int operator/(int v, LuaInteger const& rhs);


	friend bool operator== (int a, LuaInteger const& rhs); 


	friend bool operator<= (int a, LuaInteger const& rhs); 


	friend bool operator< (int a, LuaInteger const& rhs); 


	friend bool operator== (const LuaInteger li, LuaInteger const& rhs) ;


	friend bool operator<= (const LuaInteger li, LuaInteger const& rhs); 


	friend bool operator< (const LuaInteger li, LuaInteger const& rhs) ;


	friend std::ostream& operator<<(std::ostream& os, const LuaInteger& li);
};
  


//////////////////////////////////////////////////////////////////////////
// 
// struct LuaInteger2  
// {
// };
// 
// int operator+(const LuaInteger&, const LuaInteger2&)
// {
// 	return 73;
// }
// 
// struct LuaInteger3: LuaInteger {};



#endif