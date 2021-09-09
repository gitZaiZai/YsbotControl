// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef  _Servo_type_h
#define  _Servo_type_h

class ServoType
{
 
public:
	virtual ~ServoType () throw () {;}

	virtual int   read_pos( unsigned int iDeviceID, long* )   = 0;

	virtual int   clear_pos( unsigned int iDeviceID ) = 0;
	virtual int   clear_alarm( unsigned int iDeviceID ) = 0;
	virtual bool  init_port() = 0;

	virtual bool  close()  = 0;
};

#endif