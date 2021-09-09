// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

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

