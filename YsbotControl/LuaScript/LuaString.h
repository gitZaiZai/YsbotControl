#ifndef _lua_string_h
#define _lua_string_h

#include <ostream>
#include <string>

class LuaString
{
public:
	std::string m_value;
	LuaString() : m_value("") {}
	LuaString(std::string val) : m_value(val) {}
  	LuaString(const char* val) : m_value(std::string(val)) {}
	LuaString(const LuaString& lt) : m_value(lt.m_value) {}

	std::string operator+(std::string a) ;
 
	bool operator== (std::string a); 
 
	std::string operator() () const;


	std::string operator() (std::string a) ;

	friend std::string operator+(std::string a, LuaString& rhs);

	friend bool operator== (std::string a, LuaString const& rhs); 

	friend bool operator== (const LuaString li, LuaString const& rhs); 

	friend std::ostream& operator<<(std::ostream& os, const LuaString& li);
};
 



//////////////////////////////////////////////////////////////////////////




#endif