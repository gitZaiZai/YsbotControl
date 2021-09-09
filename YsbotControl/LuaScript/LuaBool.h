// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _lua_bool_h
#define _lua_bool_h
#include <ostream>

class LuaBool
{
public:
	bool m_value;
	LuaBool() : m_value(false) {}
	LuaBool(bool val) : m_value(val) {}
	LuaBool(const LuaBool& lt) : m_value(lt.m_value) {}
	const LuaBool& operator= (const LuaBool& di) 
	{
		this->m_value    = di.m_value;
 		return *this;
	}

	bool operator== (bool a) ;
	bool operator() () const;
	bool operator() (bool a) ;

	friend bool operator== (bool a, LuaBool const& rhs); 


	friend bool operator== (const LuaBool li, LuaBool const& rhs) ;


	friend std::ostream& operator<<(std::ostream& os, const LuaBool& li);


};
  

//////////////////////////////////////////////////////////////////////////




#endif