// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _Basic_Script_Variable_h
#define _Basic_Script_Variable_h

class BasicScriptVariable
{
public:
	virtual ~BasicScriptVariable(void) {;}
	virtual char* get_value( const char* name, char* strValue) =0;
 	virtual void  set_value( const char* name, char* strValue) =0;

};

#endif

