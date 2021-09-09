#ifndef _lua_real_h
#define _lua_real_h

#include <ostream>

class LuaReal
{
public:
	double m_value;
	LuaReal() : m_value(0) {}
	LuaReal(double val) : m_value(val) {}
	
	LuaReal(const LuaReal& lt) : m_value(lt.m_value) {}

	double operator+(double a) const;
 
	double operator-(double a) const;
 
	double operator*(double a) const;
 
	double operator/(double a) const;
 
	bool operator== (double a); 
 
	bool operator<= (double a); 
	 
 	bool operator< (double a); 
 
	double operator- () const;
	 
	double operator() () const;
 
	double operator() (double a); 
	 

	// 	LuaReal& operator* ()
	// 	{
	// 		return *this;
	// 	}
	friend double operator*(LuaReal const& lhs, LuaReal const& rhs);

	friend double operator+(double a, const LuaReal& rhs);

	friend double operator-(double a, const LuaReal& rhs);

	friend double operator*(double v, LuaReal const& rhs);

	friend double operator/(double v, LuaReal const& rhs);

	friend bool operator== (double a, LuaReal const& rhs); 

	friend bool operator<= (double a, LuaReal const& rhs); 

	friend bool operator< (double a, LuaReal const& rhs); 

	friend bool operator== (const LuaReal li, LuaReal const& rhs); 

	friend bool operator<= (const LuaReal li, LuaReal const& rhs); 

	friend bool operator< (const LuaReal li, LuaReal const& rhs); 


	friend std::ostream& operator<<(std::ostream& os, const LuaReal& li);
};




//////////////////////////////////////////////////////////////////////////
 



#endif