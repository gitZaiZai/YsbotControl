// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef sensable_object_h
#define sensable_object_h

#include "LuaScript/BasicScriptVariable.h"
#include "PosInfo.h"

class SensableObject : public BasicScriptVariable
{
public:
	SensableObject ();
	SensableObject (const SensableObject& ) ;
	const SensableObject& operator= (const SensableObject& ) ;
	~SensableObject(void);

	int     iCoordType; 
	int     iSensorType;
	bool   simulate_obj; 
	double xObjectPos;
	double yObjectPos;
	double zObjectPos;
	double aObjectPos;
	double bObjectPos;
	double cObjectPos;
	double width;       ///<  
	double height;      ///< z-Position 

	CartRef carRefObj;
	const CartRef refSys();

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	bool  check_value( const char* name, const bool, char* );

	CartPos get_pos();
	void set_pos(const CartPos&);
};

std::ostream& operator<< (std::ostream& os, const SensableObject& v);
std::istream& operator>> (std::istream &in, SensableObject& v);

#endif
