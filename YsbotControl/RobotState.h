// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#ifndef _RobotState_h
#define _RobotState_h

#include "MotionCoreInfo.h"
#include "PosInfo.h"

enum AxisState {
	powerOff, 
	errorStop, 
	stopping, 
	standstill, 
	discrete_motion, 
	continuous_motion, 
	synchronized_motion, 
	homing, 
};

enum ProgramRunT 
{
	prSTEP = 0,
	prONECYCLE,
	prAUTO,
};

enum SecurityT 
{ 
	smORDINARY,               
	smEDITING,
	smMANAGEMENT,
};

enum OperateMachineT 
{ 
	omREMOTE,
	omPLAY,
	omTEACH,               
};

enum JogCoordT 
{ 
	jtJOINT, // mcACS              
	jtWORLD,
	jtTOOL,
	jtUSER,
};

enum LightState 
{ 
	lsOFF = 0,
	lsON,
	lsFLASHING
};
 
enum GroupState {
	GrpMoving = 0, 
	GrpStandby,    // stop
	GrpHold,      // add by Dan
	GrpDelaying,
	GrpStopping, 
	GrpHoming, 
	GrpWait,
	GrpErrorStop, 
	GrpDisable, 
};
 
struct RobotState 
{
	SecurityT        security;            
	OperateMachineT  operate;  
	JogCoordT        coordinate;
	ProgramRunT      program_run;

	LightState run_light;
	LightState error_light;
	LightState motion_light;
	LightState process_light;
 
	GroupState       iGroupStatus;
	MotionCoreInfo   mc_info; 

	bool servo_on; 
	bool prdy_on;
	bool in_emergency;
	bool enable_press;  
	bool motor_alarm;
	bool pos_known;
	bool in_second;
	bool in_teachmoving;

	RobotState () throw ();
	RobotState (const RobotState&) throw ();
	const RobotState& operator= (const RobotState&) throw ();
};

std::ostream& operator<< (std::ostream& os, const RobotState& v);

#endif