// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "WorldModel.h"
#include <cstring>
#include <cmath>
#include <sstream>
#include "MotionControl.h"
#include "TeachDialog/TeachDlg.h"
#include "stringconvert.h"
#include "UIDialogs/InfoBox.h"
#include "UIDialogs/SettingUserDLG.h"

using namespace std;


WorldModel*  WorldModel::main_world_model = NULL;

WorldModel&  WorldModel::get_main_world_model () 
{ 
	return * WorldModel::main_world_model; 
}

bool  WorldModel::is_main_world_model_available () throw()
{
    return main_world_model != NULL;
}

WorldModel::~WorldModel () throw () 
{
 	if( WorldModel::main_world_model == this )
  		WorldModel::main_world_model = NULL;

	io_module_parameters.clear();
	
}

WorldModel::WorldModel (const  ConfigReader& vread, bool use_vision ) : configuration_list(vread),group_properties(vread),null_stream ("null.out"), rsman( vread ),
	io_module_parameters(3),external_speed(10),wait_signal(false),window_index(-1),use_vnc_desktop(false)
{
	if( WorldModel::main_world_model == NULL ) 
		WorldModel::main_world_model = this; 
 
	servo_feedback_position.valid = false;
// 	sprintf(program_cartesian_ref.selfname,"%s","Base");
// 	sprintf(jog_cartesian_ref.selfname,"%s","Base");
// 
// 	sprintf(program_tool.selfname,"%s","Default Tool-Flange");
// 	sprintf(jog_tool.selfname,"%s","Default Tool-Flange");
	curUserLevel = 0;

	prog_cart_ref.refSet = SVAR_SYSTEM;
 	prog_tool_ref.refSet = SVAR_SYSTEM;
	 
	jog_cart_ref.refSet  = SVAR_SYSTEM;
	jog_tool_ref.refSet  = SVAR_SYSTEM;

	TRACE(_T("Constuct WorldModel!\n")); 
	
}

void WorldModel::update_robot_state( const CmdInfo& cf ) throw()
{
	rsman.update( cf );
}
  
// void WorldModel::update_group_state( MotionCoreInfo::MotionStatus motion_state ) throw()
// {
// 	rsman.update( motion_state );
// }

const RobotState&  WorldModel::get_robot_state ( bool getRealTime) throw () 
{ 
	return rsman.get(getRealTime); 
}

void WorldModel::set_io_information (const InOutObject& vo, Ysbot::Time t) throw ()
{
	iobox.set (vo, t);
}

void WorldModel::set_io_module_parameters(const  IoModuleParameters& ip, const IoSource src ) throw()
{
 	io_module_parameters.at(src) = ip;
	iobox.add_io(ip, src);
}

const IoModuleParameters& WorldModel::get_io_module_parameters ( const IoSource src )  const throw ()
{
 	return io_module_parameters.at(src);
}

// in=[1..100]
void WorldModel::set_external_speed(double extpd) throw()
{
	if ( fabs(extpd-external_speed) > EPSLION )
	{
		if ( SMC.isCodeSysMotion )
		{
			double velfactor = extpd * 0.01;
			velfactor = min( max(velfactor, 0.), 1.0 ); 
			cmd.iSourceLineNo = 0;
			cmd.iCommandType = CMD_SETOVERRIDE;
			cmd.dyn.Override = velfactor;
			//SMC.setCmdInfo( cmd );
		}
	}
	external_speed = extpd;
}

void WorldModel::calc_tcp_pose( const CartPos* rob, const CartRef* cref, const CartPos* tool, CartPos* tcp,ATTITUDE_MODE am /* = AM_RPY */ )
{
	if ( SMC.isCodeSysMotion )
	{
		pose_transform.calc_tcp_pose(rob,cref,tool,tcp,AM_EULER); 
	}
	else
		pose_transform.calc_tcp_pose(rob,cref,tool,tcp,am); 
}

void WorldModel::calc_axisend_pose( const CartPos* tcp, const CartRef* cref, const CartPos* tool, CartPos* axisend, ATTITUDE_MODE am /* = AM_RPY */ )
{
	if ( SMC.isCodeSysMotion )
	{
		pose_transform.calc_axisend_pose(tcp,cref,tool,axisend,AM_EULER);
	}
	else
		pose_transform.calc_axisend_pose(tcp,cref,tool,axisend,am);
}

void WorldModel::set_key_list( const KeyObjectList& kol )
{
	key_object_list = kol; 
}

const KeyObjectList& WorldModel::get_key_list()
{
	return key_object_list;
}

void WorldModel::set_prog_cart(const RefSysSetlInfo& ref )
{
	prog_cart_ref.refSet = ref.refSet;
	prog_cart_ref.refSys = ref.refSys;
	std::string varname = prog_cart_ref.refSys.getSelfName();
	if ( teach_Dialog )
	{
		teach_Dialog->set_prog_cartref( prog_cart_ref.refSet, CString(Ansi2Unicode(varname).c_str()) );
 	}
}

void WorldModel::set_prog_tool(const RefSysSetlInfo& ref )
{
	prog_tool_ref.refSet = ref.refSet;
	prog_tool_ref.refSys = ref.refSys;
	std::string varname = prog_tool_ref.refSys.getSelfName();
	if ( teach_Dialog )
	{
		teach_Dialog->set_prog_toolref( prog_tool_ref.refSet, CString(Ansi2Unicode(varname).c_str()) );
 	}
}

void WorldModel::set_jog_cart(const RefSysSetlInfo& ref )
{
	jog_cart_ref.refSet = ref.refSet;
	jog_cart_ref.refSys = ref.refSys;
	std::string varname = jog_cart_ref.refSys.getSelfName();
	if ( teach_Dialog )
	{
		teach_Dialog->set_jog_cartref( jog_cart_ref.refSet, CString(Ansi2Unicode(varname).c_str()) );
 	}
}

void WorldModel::set_jog_tool(const RefSysSetlInfo& ref )
{
	jog_tool_ref.refSet = ref.refSet;
	jog_tool_ref.refSys = ref.refSys;

	std::string varname = jog_tool_ref.refSys.getSelfName();
	if ( teach_Dialog )
	{
		teach_Dialog->set_jog_toolref( jog_tool_ref.refSet, CString(Ansi2Unicode(varname).c_str()) );
 	}

}

bool WorldModel::is_popup_window_active()
{
	return ( ( Singleton<DigitalKeyboard>::Instance()->IsAvaliable() && Singleton<DigitalKeyboard>::Instance()->IsWindowVisible() ) ||
		     ( Singleton<FullQwerty>::Instance()->IsAvaliable() && Singleton<FullQwerty>::Instance()->IsWindowVisible()           ) ||
		     ( INFOBOX.is_main_infobox_available() && INFOBOX.IsWindowVisible() ) ||
			 ( m_pwndDlgSettingUser != NULL && m_pwndDlgSettingUser->pSetUserPopup != NULL &&
			   ::IsWindow(m_pwndDlgSettingUser->pSetUserPopup->GetSafeHwnd()) && 
			   m_pwndDlgSettingUser->pSetUserPopup->IsWindowVisible() ) );
 
}

