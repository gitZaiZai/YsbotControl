#include "StdAfx.h"
#include "RobotState.h"

 
using namespace std;

RobotState::RobotState () throw () {
	security   = smORDINARY;
	operate    = omTEACH;
	coordinate = jtJOINT;
	program_run = prONECYCLE;

	run_light     = lsOFF;
	error_light   = lsOFF;
	motion_light  = lsOFF;
	process_light = lsOFF;

	iGroupStatus = GrpDisable;
 	servo_on       = false; 
	prdy_on        = false;
	in_emergency   = false;
	enable_press   = false;  
	motor_alarm    = false;
	pos_known      = false; 
	in_second      = false;
	in_teachmoving = false;
}

RobotState::RobotState (const RobotState& gs) throw () {
	(*this) = gs;
}

const RobotState& RobotState::operator= (const RobotState& rs) throw () {
	security   =  rs.security;
	operate    =  rs.operate;
	coordinate =  rs.coordinate;
	program_run = rs.program_run;

	run_light     =  rs.run_light;
	motion_light  =  rs.motion_light;
	error_light   =  rs.error_light;
	process_light =  rs.process_light;

	mc_info        =  rs.mc_info;
	servo_on       =  rs.servo_on; 
	prdy_on        =  rs.prdy_on;
	in_emergency   =  rs.in_emergency;
	enable_press   =  rs.enable_press;  
	motor_alarm    =  rs.motor_alarm;
	
	iGroupStatus   = rs.iGroupStatus;
	pos_known      = rs.pos_known;
	in_second      = rs.in_second;
	in_teachmoving     = rs.in_teachmoving;

	return (*this);
}

std::ostream& operator<< (std::ostream& os, const RobotState& v) 
{
	// 	static_cast<int>(v.pos_type)
	switch(v.iGroupStatus)
	{
	case GrpMoving:   os << " GrpMoving "   << ' '; break;
	case GrpStandby:  os << " GrpStandby "  << ' ';break;
	case GrpHold:     os << " GrpHold "     << ' ';break;
	case GrpDelaying: os << " GrpDelaying " << ' ';break;
	case GrpStopping: os << " GrpStopping " << ' ';break; 
	case GrpHoming:   os << " GrpHoming "   << ' ';break;
	case GrpWait:     os << " GrpWait "     << ' ';break;
	case GrpErrorStop:os << " GrpErrorStop " << ' '; break;
	case GrpDisable:  os << " GrpDisable "   << ' ';break;
	default: break;
	}

	switch(v.mc_info.iMotionStatus)
	{
	case MotionCoreInfo::msRUNNING:  os << " msRUNNING "  << ' '; break;
	case MotionCoreInfo::msSTOP:     os << " msSTOP "     << ' '; break;
	case MotionCoreInfo::msHOLD:     os << " msHOLD "     << ' '; break;
	case MotionCoreInfo::msDELAYING: os << " msDELAYING " << ' '; break;
 	default: break;
	}
	return os;
}