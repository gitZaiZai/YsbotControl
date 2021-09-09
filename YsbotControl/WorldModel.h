// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _world_model_h
#define _world_model_h

#include <iostream>
#include <fstream>
#include "ConfigReader.h"
#include "RobotStateManager.h"
#include "IoModuleParameters.h"
#include "InOutContainer.h"
#include "CmdInfo.h" 
#include "MotionCoreInfo.h"
#include "ProgCommandInfo.h"
#include "PoseTransform.h"
#include "KeyObjectList.h"
#include "LuaScript/ScriptVariableType.h"
#include "UIDialogs/UserData.h"

struct RefSysSetlInfo
{
	ScriptVarSetT refSet;
 	CartRef refSys;
};

class WorldModel
{
 public:
    static WorldModel& get_main_world_model ();
    static bool is_main_world_model_available ();
	inline std::ostream& log_stream ()  { return null_stream; }

    WorldModel (const  ConfigReader&, bool use_vision = false );
    ~WorldModel ();

	inline const GroupProperties& get_group_properties () const throw () { return group_properties; }
	// iMotionStatus
	// 0 GMS_RUNNING:  executing a motion command
	// 1 GMS_STOP:     stopping status and none of command exist in a command queue
	// 2 GMS_HOLD:     holding status (because usig MCC_HoldMotion())  and some commands exist in a command queue
	// 3 GMS_DELAYING: delay status   (because usig MCC_DelayMotion()) and some commands exist in a command queue
	const RobotState& get_robot_state ( bool getRealTime=false) throw ();
 	const InOutObjectList& get_io_objects (const IoSource ice ) throw () { return iobox.get_io_objects(ice); }
	const IoModuleParameters& get_io_module_parameters (const IoSource =IOSfieldbus)  const throw ();
 
 	inline void set_group_properties (const GroupProperties& rpr) throw () { group_properties = rpr; }
	inline void set_motion_core_info (const MotionCoreInfo& gif) throw () { main_motion_core_info = gif; rsman.set_motion_core_info(gif); }
	const MotionCoreInfo& get_motion_core_info () throw () { return main_motion_core_info; }

	void set_io_module_parameters (const  IoModuleParameters&, const IoSource =IOSfieldbus) throw ();
	void set_servo_on(  bool b )          { rsman.set_servo_on(b);  }
	void set_prdy_on(   bool b )          { rsman.set_prdy_on(b);   }
	void set_startpos_known ( bool b )    { rsman.set_startpos_known(b); }
	inline void set_enable ( bool b )     { rsman.set_enable(b);    }   
	void set_emergency( bool b )          { rsman.set_emergency(b); }   
	inline void set_motor_alarm( bool b)  { rsman.set_motor_alarm(b); }
	inline void set_in_second (bool b)    { rsman.set_in_second(b);   }
	inline void set_teach_moving(bool b) { rsman.set_teach_moving(b); }
	inline void set_next_pos(PosInfo p)  { next_pos = p; }
	inline void set_last_pos(PosInfo p)  { last_pos = p; }
	inline void set_teachmove_type(unsigned int t) {teach_moving_type=t;}
	PosInfo get_next_pos()				 {return next_pos;}
	PosInfo get_last_pos()				 {return last_pos;}
	unsigned int get_teachmove_type()	 {return teach_moving_type;}
	void setCurUserLevel(int userLevel)	 { curUserLevel = userLevel;}
	int getCurUserLevel()				 { return curUserLevel;}

	inline void set_run_light ( LightState ls )  { rsman.set_run_light(ls); }
	inline void set_error_light ( LightState ls )  { rsman.set_error_light(ls); }
	inline void set_motion_light ( LightState ls )  { rsman.set_motion_light(ls); }
	inline void set_process_light ( LightState ls )  { rsman.set_process_light(ls); }

	void set_jog_coordinate ( JogCoordT jcs  ) { rsman.set_jog_coordinate(jcs); }
	void set_operation_mode ( OperateMachineT omd ) { rsman.set_operation_mode(omd); }         
	void set_security_mode (  SecurityT scm  ) { rsman.set_security_mode(scm);  }
	void set_programrun_mode ( ProgramRunT pt) { rsman.set_programrun_mode(pt); }

	void set_io_information (const InOutObject&, Ysbot::Time) throw ();

	void update_robot_state ( const CmdInfo& cf ) throw();
// 	void update_group_state( MotionCoreInfo::MotionStatus ) throw();
	void  set_external_speed(double extpd) throw ();
	inline const double get_external_speed()const throw () { return external_speed; }

	inline void  set_prog_command_info( ProgCommandInfo proinfo )     { prog_cmd_info = proinfo; }
	inline const ProgCommandInfo& get_prog_command_info() const throw () { return prog_cmd_info;    }

	inline void  set_wait_signal( bool b )     { wait_signal = b; }
	inline const bool& get_wait_signal() const throw () { return wait_signal;    }

	void calc_tcp_pose ( const CartPos* rob, const CartRef* cref, const CartPos* tool, CartPos* tcp,ATTITUDE_MODE am = AM_RPY );
	void calc_axisend_pose ( const CartPos* tcp, const CartRef* cref, const CartPos* tool, CartPos* axisend, ATTITUDE_MODE am = AM_RPY );

	void set_window_index(int id) {window_index = id;}
	int  get_window_index() { return window_index; }

//-----------------------------------------------------------------------------------------
// 	void set_programref_cartesian(const CartRef& cref) { program_cartesian_ref = cref; }
// 	const CartRef& get_programref_cartesian() { return program_cartesian_ref; }
// 
// 	void set_programref_tool(const CartRef& tool ) { program_tool = tool; }
// 	const CartRef& get_programref_tool() { return program_tool; }
// 
// 	void set_jogref_cartesian(const CartRef& cref) { jog_cartesian_ref = cref; }
// 	const CartRef& get_jogref_cartesian() { return jog_cartesian_ref; }
// 
// 	void set_jogref_tool(const CartRef& tool ) { jog_tool = tool; }
// 	const CartRef& get_jogref_tool() { return jog_tool; }

	void set_prog_cart(const RefSysSetlInfo& ref );
	void set_prog_tool(const RefSysSetlInfo& ref );
	void set_jog_cart(const RefSysSetlInfo& ref );
	void set_jog_tool(const RefSysSetlInfo& ref );

	const RefSysSetlInfo& get_prog_cart( ) { return prog_cart_ref; }
	const RefSysSetlInfo& get_prog_tool( ){ return prog_tool_ref; }
	const RefSysSetlInfo& get_jog_cart( ) { return jog_cart_ref; }
	const RefSysSetlInfo& get_jog_tool( ) { return jog_tool_ref; }
//-----------------------------------------------------------------------------------------

	inline void  set_vision_pos( const VisionPos& vp )     { vispos = vp; }
	inline const VisionPos& get_vision_pos() const throw () { return vispos;    }

	inline void  set_servo_position( const PosInfo& ap )     { servo_feedback_position = ap; }
	inline const PosInfo& get_servo_position() const throw () { return servo_feedback_position;    }

//-----------------------------------------------------------------------------------------
    inline void set_dynamic_program( const Dynamic& dyn ) { dynamic_program = dyn; }
	inline const Dynamic& get_dynamic_program() { return dynamic_program; }

	inline void set_overlap_program( const Overlap& ove ) { overlap_program = ove; }
	inline const Overlap& get_overlap_program() { return overlap_program; }

//-----------------------------------------------------------------------------------------
	void set_key_list( const KeyObjectList& kol );
	const KeyObjectList& get_key_list();

	void set_teach_command( const CmdInfo& cmd ) { teach_command = cmd; }
	CmdInfo& get_teach_command() { return teach_command; }

	bool is_popup_window_active ();

	void set_userdata(const UserData& udata)	 { user_data = udata; }
	inline const UserData& get_userdata()		 { return user_data; }

	inline void  set_vnc_desktop ( bool b )     { use_vnc_desktop = b; }
	inline const bool& is_vnc_desktop () const throw () { return use_vnc_desktop;    }

protected:
	std::ofstream		null_stream; 

private:
    const ConfigReader&	 configuration_list;
    static WorldModel*	 main_world_model;
	GroupProperties      group_properties;
	MotionCoreInfo       main_motion_core_info;
	RobotStateManager    rsman;

	std::vector<IoModuleParameters> io_module_parameters;
	
	InOutContainer       iobox;
	double               external_speed;
	ProgCommandInfo      prog_cmd_info;
	bool                 wait_signal;

	PoseTransform        pose_transform;

	int window_index;
 
	RefSysSetlInfo prog_cart_ref;
	RefSysSetlInfo prog_tool_ref;
	RefSysSetlInfo jog_cart_ref;
	RefSysSetlInfo jog_tool_ref;

	VisionPos vispos;
	PosInfo servo_feedback_position;

	PosInfo next_pos;
	PosInfo last_pos;
	unsigned int teach_moving_type; 

	int curUserLevel;

	Dynamic dynamic_program;
	Overlap overlap_program;

	KeyObjectList key_object_list;
	CmdInfo cmd;
	CmdInfo teach_command;
	UserData user_data;
	bool use_vnc_desktop;
};

#define MWM  WorldModel::get_main_world_model()
#define LOUT WorldModel::get_main_world_model().log_stream()

#endif

