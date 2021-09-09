#ifndef _RobotStateManager_h
#define _RobotStateManager_h
#include "RobotState.h"
#include "ConfigReader.h"
#include "GroupStateMachine.h"

class RobotStateManager
{
public:
	RobotStateManager(const ConfigReader&);
	~RobotStateManager(void);
	const RobotState& get (bool =false) throw ();

// 	void update (MotionCoreInfo::MotionStatus) throw ();

	void update (const CmdInfo&) throw ();

	void set_servo_on( bool );
	void set_prdy_on( bool );
	void set_enable( bool );  
	void set_emergency( bool );  
	void set_startpos_known( bool b)          { rs.pos_known   = b; }
	inline void set_motor_alarm( bool b) { rs.motor_alarm = b; }  

	inline void set_in_second(bool b) { rs.in_second = b; }
	inline void set_teach_moving(bool b) { rs.in_teachmoving = b; }

	inline void set_run_light( LightState ls )  { rs.run_light = ls; }
	inline void set_error_light( LightState ls )  { rs.error_light = ls; }
	inline void set_motion_light( LightState ls )  { rs.motion_light = ls; }
	inline void set_process_light( LightState ls )  { rs.process_light = ls; }

	void set_jog_coordinate( JogCoordT );
	void set_operation_mode( OperateMachineT );
	void set_security_mode( SecurityT );
	void set_programrun_mode( ProgramRunT );

	void set_motion_core_info( const MotionCoreInfo& );

private:
	RobotState rs;                 
 	GroupStateMachine gsm;    
};

#endif