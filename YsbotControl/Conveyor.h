// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#ifndef _conveyor_h
#define _conveyor_h

#include "LuaScript/BasicScriptVariable.h"
#include "PosInfo.h"
#include "SensableObject.h"

class Conveyor : public BasicScriptVariable
{
public:
	int     iCoordType;  // PCS_1 or 2
	int     iConveyorType; // belt or table rotate.
	double  belt_speed;
	double  belt_acceleration;
	double  belt_deceleration;
	double  belt_jerk;
	double  min_work_area;
	double  max_work_area;

	double xBeltOrg;
	double yBeltOrg;
	double zBeltOrg;
	double aBeltOrg;
	double bBeltOrg;
	double cBeltOrg;
 
	double xObjInitPos;
	double yObjInitPos;
	double zObjInitPos;
	double aObjInitPos;
	double bObjInitPos;
	double cObjInitPos;

	Conveyor ();
 	Conveyor (const Conveyor& di) ;
	const Conveyor& operator= (const Conveyor& di) ;
 
	void begin_track () throw ();
	void end_track (  ) throw();
	int  wait (SensableObject*) throw();
	void done( SensableObject*,bool) throw();
	const CartRef refSysObject();

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	bool  check_value( const char* name, const bool, char* );
    void  set_obj_initpos(const CartPos& objpos);

	CartRef carRefObj;
};

std::ostream& operator<< (std::ostream& os, const Conveyor& v);
std::istream& operator>> (std::istream &in, Conveyor& v);


#endif