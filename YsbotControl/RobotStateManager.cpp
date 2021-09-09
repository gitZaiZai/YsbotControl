#include "StdAfx.h"
#include "RobotStateManager.h"
#include "MotionControl.h"

RobotStateManager::RobotStateManager(const ConfigReader&)
{
	rs.security   = smORDINARY;
	rs.operate    = omTEACH;
	rs.coordinate = jtJOINT;
	rs.program_run = prAUTO;
	rs.run_light     = lsOFF;
	rs.error_light   = lsOFF;
	rs.motion_light  = lsOFF;
	rs.process_light = lsOFF;

	rs.iGroupStatus = GrpDisable;
	rs.servo_on       = false; 
	rs.prdy_on        = false;
	rs.in_emergency   = false;
	rs.enable_press   = false;  
	rs.motor_alarm    = false;
 
}


RobotStateManager::~RobotStateManager(void)
{
	;
}

const RobotState& RobotStateManager::get ( bool getRealTimeMotionInfo )  throw ()
{
	if ( getRealTimeMotionInfo )
	{
   		rs.mc_info = SMC.get_motion_core_information();
	}
	return rs;
}

void RobotStateManager::update (const CmdInfo& cf) throw () 
{
 	gsm.update (cf);
	rs.iGroupStatus = gsm.get_state(); 
}

// void RobotStateManager::update( MotionCoreInfo::MotionStatus motion_state ) throw()
// {
// 	gsm.update( motion_state );
// 	rs.iGroupStatus = gsm.get_state();
// }

void RobotStateManager::set_servo_on (bool b)  // ×´Ì¬ÏÔÊ¾
{
	rs.servo_on = b;
}

void RobotStateManager::set_prdy_on( bool b)
{
	rs.prdy_on = b;
 }

void RobotStateManager::set_enable(bool b)  
{
	rs.enable_press = b;
}

void RobotStateManager::set_emergency(bool b)  
{
	rs.in_emergency = b;
}

void RobotStateManager::set_security_mode( SecurityT sm)
{
	rs.security = sm;
}

void RobotStateManager::set_operation_mode( OperateMachineT om)
{
	rs.operate = om;
}

void RobotStateManager::set_jog_coordinate( JogCoordT ct )
{
	rs.coordinate = ct;
}

void RobotStateManager::set_programrun_mode(ProgramRunT pt )
{
    rs.program_run = pt;
}

void RobotStateManager::set_motion_core_info( const MotionCoreInfo& gf )
{
	rs.mc_info = gf;
	gsm.update( gf.iMotionStatus );
	rs.iGroupStatus = gsm.get_state();
}