// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _IEC_BASIC_TYPES_
#define _IEC_BASIC_TYPES_

//#define  USE_MACHINE_X86
// #define USE_MACHINE_X86 0   // GDPC 32bit
// #define USE_MACHINE_X64 1   // YS_PC 64bit
#include "../Angle.h"
typedef unsigned __int64 ULINT; // 8 bytes, 64 bit

//#define ARRAY_SIZE(a)			(sizeof(a) / sizeof((a)[0]))

namespace Ysbot {

// #pragma pack(push) //保存对齐状态
// #pragma pack(4)//设定为4字节对齐

	struct MC_AXISPOS_REF
	{
		double a0;
		double a1;
		double a2;
		double a3;
		double a4;
		double a5;
	};

	struct MC_COORD_REF
	{
		double X;
		double Y;
		double Z;
		double A; // z   phi
		double B; // y'  theta
		double C; // z'' psi
	};

	union SMC_POS_REF
	{
		MC_AXISPOS_REF apos;
		MC_COORD_REF   coordRef;
		double         valArray[6];
	};

	struct SMC_DYN_LIMITS 
	{
		double fVelMax;
		double fAccMax;
		double fDecMax; 
		double fJerkMax;
	};

// #pragma pack(pop) //恢复对齐状态

	enum SMC_AXIS_GROUP_STATE  {
		Disabled = 0,
		Standby,
		Moving,
		Homing,
		Stopping,
		ErrorStop,

		GSILLEGAL
	};

	enum SMC_AXIS_STATE  {
		power_off=0,
		errorstop=1,   
		stopping=2,   
		standstill=3,   
		discrete_motion=4,   
		continuous_motion=5,   
		synchronized_motion=6,   
		homing=7
	};

	static const char group_state_names [7][12] = 
	{         
		"Disabled  ",
		"Standby   ",
		"Moving    ",
		"Homing    ",
		"Stopping  ",
		"ErrorStop ",
		"GSILLEGAL "

	};

// 	static const char error_description [10][23] = 
// 	{         
// 		"No            error  ",
// 		"Communication error  ",
// 		"Axis error           ",
// 		"Fieldbus      error  ",
// 		"command timeout      ",
// 		"demo expired         ",
// 		"dd error             ",
// 
// 	};

	static struct {
		int      nErrorCode;
		const char*  lpszDesc;
	}
	error_description[] = {
		2,		 "Axis error",
		50100,	 "Demo mode expired!",
		50101,   "External emgc stop!",
		50102,   "robot type error!",
		50103,   "group kine axis num error",
		50104,	 "Execute command timeout!",
		11000,   "The axis group is in the wrong state for the requested operation",
		11005,   "An axis is in state error,may be Over Soft Limit ",
		11014,   "Servo off, error occurred follow the computed set values",
		11021,   "A parameter of an administrative function block is invalid",
		18,      "Soft motion Demo mode License expired!"
	};

	enum SMC_COORD_SYSTEM {
		ACS = 0,
		MCS,
		WCS,
		PCS_1,
		PCS_2,
		TCS,
		CSILLEGAL
	};

	static const char coord_system_names [6][12] = 
	{         
		"ACS       ",
		"MCS       ",
		"WCS       ",
		"PCS_1     ",
		"PCS_1     ",
		"CSILLEGAL "

	};

	enum MC_ERROR_TYPE {
		ERR_NO,
		ERR_SMC,
		ERR_CONNECT,
		ERR_COMMAND,
		ERROR_DEMO_EXPIRED, 
		ERROR_UNKNOWN,
	};

}
#endif