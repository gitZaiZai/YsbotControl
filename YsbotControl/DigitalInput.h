// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _digital_input_h
#define _digital_input_h
#include "LuaScript/BasicScriptVariable.h"

class DigitalInput : public BasicScriptVariable
{
public:
	int  device_id;
	int  port;
	int  input_value;
	int  wait_time;
	bool virtual_input;

	DigitalInput () : device_id(0), port(0), input_value(0), wait_time(0),virtual_input(false) {;}
	DigitalInput (int _id, int pnb, int inval, long wt, bool vi ) : device_id(_id), port(pnb), input_value(inval), wait_time(wt),virtual_input(vi) {;}
	DigitalInput (const DigitalInput& di) { operator= (di); }
 	const DigitalInput& operator= (const DigitalInput& di) 
	{
		device_id   = di.device_id;
		port        = di.port;
		input_value = di.input_value;
		wait_time   = di.wait_time;
		virtual_input = di.virtual_input;
 		return *this;
	}

	int read () throw ();
	int wait ( bool, int ) throw();
	int waitbit (int, int ) throw();
	int readbit () throw();
	int read_while_motion (double) throw();
	int readNoWait () throw();

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	void  check_value( const char* name, const bool, char* );
};

std::ostream& operator<< (std::ostream& os, const DigitalInput& v);
std::istream& operator>> (std::istream &in, DigitalInput& v);


#endif