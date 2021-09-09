// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "CodeSysMotion.h"
#include "../MotionControlFactory.h"
#include "../YsibotsException.h"
#include "../Journal.h"
#include "../WorldModel.h"
#include "../random.h"

using namespace Ysbot;

namespace
{
	class Builder : public MotionControlBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			MotionControlFactory::get_motion_control_factory ()->sign_up (std::string("CodeSys"), this);
		}
		MotionControlType* get_motion_control (const std::string&, const ConfigReader& reader, MotionControlType*) throw (std::bad_alloc) {
			return new CodeSysMotion (reader);
		}
	};
	Builder the_builder;
}

CodeSysMotion::CodeSysMotion (const ConfigReader& reader) throw (InvalidConfigurationException) : bJogMotion(false),reset_position(false) 
{
	TRACE(_T("Constuct CodeSysMotion!\n")); 
	string addrHost   = "127.0.0.1";
	robot_type = 3; // 1 : tripod, 3 : six_robot_virtual
	string confline;

	if (reader.get("CodeSysMotion::motion_ip_address", confline) > 0 ) 
	{
		addrHost = confline;
 	}

	reader.get("CodeSysMotion::RobotType", robot_type); 

// 	std::string addrGDPC   = "192.168.11.168"; // epico 168 GD 167
// 	std::string addrNotePC = "192.168.11.153";
	addrCurr   = addrHost; 

#ifdef USE_MACHINE_X86
	cout<<"run on GDPC machine x32"<<endl;
// 	robot_type = 4; // 1 : tripod, 3 : six_robot_virtual, 4: six_robot_sanyo
// 	addrCurr  = addrGDPC;
#else
	cout<<"run on machine x64"<<endl;
// 	robot_type = 3; // 1 : tripod, 3 : six_robot_virtual, 4: six_robot_sanyo
// 	addrCurr  = addrHost;
	// 	addrCurr  = addrNotePC;
#endif 

	unsigned int ex_num = 0;
	unsigned int ey_num = 0;
	bool use_etcio = false;
	unsigned int cycle = 0;

	reader.get("CODESYS_ETC_IO::digital_input_num", ex_num); //???????????????????????
	reader.get("CODESYS_ETC_IO::digital_ouput_num", ey_num);	 
	reader.get("CODESYS_ETC_IO::use_etc_io",  use_etcio);
	reader.get("CODESYS_ETC_IO::scan_task_cycle",  cycle);

	int ex_word_size = static_cast<int>( ceil( static_cast<double>(ex_num) / 16.0 ) );
	int ey_word_size = static_cast<int>( ceil( static_cast<double>(ey_num) / 16.0 ) );

	iomodule_para.is_connected = use_etcio;
	iomodule_para.scan_cycle_time = cycle;
	iomodule_para.x_point_num = ex_num;
	iomodule_para.y_point_num = ey_num;
	iomodule_para.x_word_size = ex_word_size;
	iomodule_para.y_word_size = ey_word_size;

	iomodule_para.local_input_size = 4;

	bConnectSuccess = false;
	ZeroMemory(&client_command,sizeof(Ysbot::ClientCommand));
	ZeroMemory(&server_data,sizeof(Ysbot::IECServerData));
	ZeroMemory(&server_data_from_shm,sizeof(Ysbot::IECServerData));

	// initialise command which send to client.
	client_command.Velocity     = 10; // max is 100
	client_command.Acceleration = 6000; 
	client_command.Deceleration = 6000;
	client_command.Jerk         = 1e5;
	client_command.AccFactor  = 0.1;
	client_command.JerkFactor = 1.0;
	
 	client_command.nMaxElapsedTime = 5000;

	client_command.RelDistance.coordRef.X = 0;
	client_command.RelDistance.coordRef.Y = 0;
	client_command.RelDistance.coordRef.Z = 0;
	client_command.RelDistance.coordRef.A = 0;
	client_command.RelDistance.coordRef.B = 0;
	client_command.RelDistance.coordRef.C = 0;
	client_command.BufferMode = Buffered;
	client_command.TransMode  = TMNone;
	client_command.TransParam[0] = 0;
	client_command.TransParam[1] = 0;

	max_path_dyn.fVelMax = 100;
	max_path_dyn.fAccMax = 1000;
	max_path_dyn.fDecMax = 1000;
	max_path_dyn.fJerkMax = 100000.0;

	pose_trans.set_interpolation_point_num(12);
 	
	reader.get("CodeSysMotion::reset_homeposition", reset_position);
	for ( int i = 0; i < MAX_JOINT_NUM; ++i )
	{
		encoder_pos[i] = 0.0;
	}	

	if ( reset_position )
	{
		unsigned int num = 0;

		std::vector<std::string> strAxis;
		strAxis.clear();

		reader.get("joints", strAxis);
		if ( strAxis.size() == 0 )
			throw InvalidConfigurationException ("joints home");
		else
			num = strAxis.size();
 
		for ( unsigned int j = 0; j < num; j++ )
		{
 			reader.get((strAxis[j]+string("::encoder_deg")).c_str(), encoder_pos[j] );
 		}

		strAxis.clear();
	} 
	else
	{

	}



}

CodeSysMotion::~CodeSysMotion()
{
}

int CodeSysMotion::init_system()
{
	bConnectSuccess = creat_connect( addrCurr.c_str() );
	int ret = 0;
	if ( bConnectSuccess )
	{
		printf(" connect successful \n");

		robot_client.start_task_thread();
  
		set_robot_type(robot_type); // 2 sanyo driver, define MACHINE_X86 
 		enable_robot_group(true);
 		set_max_path_dyn();
 		set_max_joint_dyn();

		CmdInfo cmd;
		if (MWM.is_main_world_model_available())
 			cmd.dyn.Override = MWM.get_external_speed() * 0.01;
 		else
			cmd.dyn.Override = 0.5;
		//set_group_override(cmd);


		if ( reset_position )
		{
			cmd.iCommandType = CMD_SETPOSITION;
			cmd.customStartPos = true;
			cmd.startPos.axis.set_deg( encoder_pos[0], encoder_pos[1], encoder_pos[2], encoder_pos[3], encoder_pos[4], encoder_pos[5] );
			set_position(cmd);
		}
 	}
	else
	{
		TRACE(_T("creat connect error!"));
		ret = -1;
	}
	return ret;
}

int CodeSysMotion::close_system()
{
	TRACE(_T("close CodeSysMotion!\n")); 
	return 0;
}

int CodeSysMotion::setCmdInfo( const CmdInfo& cf )
{
	int ret = 0;
	client_command.iSourceLineNo = cf.iSourceLineNo;
	switch( cf.iCommandType )
	{
	case CMD_JOG:			jog_motion(cf);      break;
	case CMD_PTP:			move_ptp(cf);        break;
	case CMD_PTPREL:        move_ptp_rel(cf);    break;
	case CMD_LIN:			move_line(cf);       break;
	case CMD_LINREL:        move_line_rel(cf);   break;
	case CMD_CIR:			move_circle(cf);     break;
	case CMD_CIRREL:        move_circle_rel(cf); break;
	case CMD_WAITIO:		wait_input(cf);      break;
	case CMD_DELAYING:		delay_motion(cf);    break;
	case CMD_HALT:			hold_motion();       break;
	case CMD_CONTINUE:		continue_motion();   break;
	case CMD_ABORT:			stop_motion(cf);     break;
	case CMD_ABORTJOG:		stop_jog(cf);        break;
	case CMD_RESET:			reset_group();       break;
	case CMD_POWER:			power_servo(cf);     break;
	case CMD_SETPOSITION:	set_position(cf);    break;
	case CMD_SETOUTPUT:		set_digital_ouput(cf); break;
	case CMD_BLEND:        ;     break;
	case CMD_SPLINE:       ;     break;
	case CMD_RESETCMDINDEX: ;    break;
	case CMD_TIMEDELAY:     ;    break;
	case CMD_SETOVERRIDE: set_group_override(cf); break;
	case CMD_WAITMOTIONFINISH: wait_finished();   break;
	case CMD_SETCOORDREF : SetCoordinateTransform(cf); break;

	case CMD_CONVEYORBEGIN: conveyor_begin(cf); break;
	case CMD_CONVEYORWAIT: ret = conveyor_wait(cf); break;
	case CMD_CONVEYORDONE: conveyor_done(cf); break;
	case CMD_CONVEYOREND: conveyor_end(cf); break;

	default:break;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//  called by child thread, use to show io state in UI.

int CodeSysMotion::read_input( unsigned short* pVal, unsigned short io_type )
{
	int ret    = 0;
	if ( io_type == MCS_REMOTEIO )  
	{
 		int word_size  = iomodule_para.x_word_size;
		robot_client.get_server_data(&server_data);
 		for ( int i = 0; i < word_size; i++ )
		{
			*(pVal+i) = server_data.wInputValue[i];
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// use to operator IO while program run.

int CodeSysMotion::read_word( unsigned short port, unsigned short* val, unsigned short io_type/* =0 */ )
{
	int ret     = 0;
 	int set_idx = 0;

	if ( io_type == MCS_GPIO )
	{
  		*val = 0;
	}
	else if( io_type == MCS_REMOTEIO )
	{
		robot_client.get_server_data(&server_data);
 		*val = server_data.wInputValue[port];
	}
	else if ( io_type == MCS_LOCALIO ) // 0~7 POT+, 10~17 NOT-, 20~27 HOME
	{
		*val = 0;
	}

 	return ret;
}

int CodeSysMotion::write_word( unsigned short port, unsigned short val, unsigned short io_type, bool inqueue, double dfDelayTime )
{
	int ret     = 0;

 	if ( io_type == MCS_GPIO )
	{
       ;
	}
	else if ( io_type == MCS_REMOTEIO ) // ?????????????????????????
	{
 		int word_size  = iomodule_para.y_word_size;
		if( port >=0 && port < word_size )
		{
			if ( inqueue )
			{
				wait_finished();
			}
 
			client_command.CmdType = IEC_CMD_SETDIGITALOUTPUT;
			client_command.wPortIndex = port;
			client_command.wPortValue = val;
			robot_client.putCommandInQueue(client_command);

		}
  	}
	else if ( io_type == MCS_LOCALIO ) // servo on, posRdy
	{
		;
	}

	return ret;
}

bool CodeSysMotion::creat_connect(const char * addr)
{
	bool ret = robot_client.init(4711,addr);
 	if ( ret )
	{
		ret = robot_client.connect_to_server();
	}
	return ret;
}

void CodeSysMotion::set_robot_type( int iType )
{
	client_command.CmdType = IEC_CMD_SETROBOTTYPE;
	client_command.iParams[0] = iType; // 0-TripodLinear, 1-TripodRotary,
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::enable_robot_group( bool bEnable )
{
	client_command.CmdType = bEnable? IEC_CMD_GROUPENABLE : IEC_CMD_GROUPDISABLE;
 	robot_client.putCommandInQueue(client_command);
}
 
void CodeSysMotion::SetCoordinateTransform( const CmdInfo& cf )
{
	client_command.CmdType = IEC_CMD_SETCOORD;
	CartPos workcart = cf.cartref->cpos;
	client_command.EndPos.coordRef.X = workcart.pos.x;
	client_command.EndPos.coordRef.Y = workcart.pos.y;
	client_command.EndPos.coordRef.Z = workcart.pos.z;
	client_command.EndPos.coordRef.A = rad2deg( workcart.ang.x );
	client_command.EndPos.coordRef.B = rad2deg( workcart.ang.y );
	client_command.EndPos.coordRef.C = rad2deg( workcart.ang.z );
	robot_client.putCommandInQueue(client_command);

}

void CodeSysMotion::jog_motion( const CmdInfo& cmd_info )
{
	jog_motion_group( cmd_info );
}

void CodeSysMotion::jog_motion_group( const CmdInfo& cmd_info )
{
	if ( bJogMotion )
	{
		return;
	}
	else
	{
		bJogMotion = true;
		cout << "----------- jog axis control ------ " << '\n';
	}

	int nAxis = cmd_info.chJogAxisMark;
	int dir   = cmd_info.chJogDirMark;
	bool isHaveTool  = false;
	bool isRotateTcp = false;
	
	if ( cmd_info.tool && ( cmd_info.tool->pos != Vec3D(0,0,0) || cmd_info.tool->ang != Vec3D(0,0,0) ) )
	{
		isHaveTool = true;
	}
	if ( nAxis >= 8 && nAxis < 64  )
	{
		isRotateTcp = true;
	}

	const GroupProperties& gp = MWM.get_group_properties();
	const double max_teach_vel= 20; // gp.profile.dfMaxExternalVel; // 0-100
	double curr_vel_percent   = MWM.get_external_speed();
	double actual_vel_percent = max_teach_vel * curr_vel_percent * 0.01;
	double rate = min( max( actual_vel_percent * 0.01, 0.0 ), max_teach_vel * 0.01 );  //kv=[0..0.15]
	
	client_command.CmdType = IEC_CMD_JOGGROUP; // CMD_JOGAXIS;
  	client_command.VelFactor    = 1.0; // actual_vel_percent * 0.01;
	client_command.AccFactor    = 1.0;
	client_command.JerkFactor   = 1.0;

	client_command.wJogDirMark  = cmd_info.chJogDirMark;
	client_command.wJogAxisMark = cmd_info.chJogAxisMark;
	client_command.nMaxElapsedTime = 0;

// 	const double kav = -1.0/ 0.12 * rate + max_teach_vel*0.01*7.3+2;
	const double kav =  2.0/exp(3*rate)+2;
 
	if ( cmd_info.iJogCoordType == jtJOINT )
	{
		client_command.CoordSystem = ACS;

 		client_command.Velocity     = rate * max_joint_dyn[0].fVelMax; // axis 1: 150
 		client_command.Acceleration = kav * rate*rate*max_joint_dyn[0].fAccMax; // 750
		client_command.Deceleration = kav * rate*rate*max_joint_dyn[0].fDecMax; // 750
		client_command.Jerk         = 100000;// kav * rate*rate*max_joint_dyn[0].fJerkMax; // 1500

	}
	else
	{ 
 		client_command.Velocity     = rate * max_path_dyn.fVelMax; // 2000
 		client_command.Acceleration = kav * rate*rate*max_path_dyn.fAccMax; // 5000
		client_command.Deceleration = kav * rate*rate*max_path_dyn.fDecMax;
		client_command.Jerk         = 100000;//kav * rate*rate*max_path_dyn.fJerkMax;

		// XYZ RXRYRZ none tool WCS
		client_command.EndPos.coordRef.X = 0;
		client_command.EndPos.coordRef.Y = 0;
		client_command.EndPos.coordRef.Z = 0;
		client_command.EndPos.coordRef.A = 0;
		client_command.EndPos.coordRef.B = 0;
		client_command.EndPos.coordRef.C = 0;
 	    // TCS
		client_command.RelDistance.coordRef.X = 0;
		client_command.RelDistance.coordRef.Y = 0;
		client_command.RelDistance.coordRef.Z = 0;
		client_command.RelDistance.coordRef.A = 0;
		client_command.RelDistance.coordRef.B = 0;
		client_command.RelDistance.coordRef.C = 0;
  
		if ( cmd_info.iJogCoordType == jtTOOL )
		{
			client_command.CoordSystem = TCS;
			if ( isHaveTool )
			{
				client_command.RelDistance.coordRef.X = cmd_info.tool->pos.x;
				client_command.RelDistance.coordRef.Y = cmd_info.tool->pos.y;
				client_command.RelDistance.coordRef.Z = cmd_info.tool->pos.z;
				client_command.RelDistance.coordRef.A = rad2deg( cmd_info.tool->ang.x );
				client_command.RelDistance.coordRef.B = rad2deg( cmd_info.tool->ang.y );
				client_command.RelDistance.coordRef.C = rad2deg( cmd_info.tool->ang.z );
			}		
		}
		else if ( cmd_info.iJogCoordType == jtWORLD )
		{
			client_command.CoordSystem = MCS;
		}
		else if ( cmd_info.iJogCoordType == jtUSER )
		{
			client_command.CoordSystem = WCS;
			CartPos workCoord;
			if ( cmd_info.cartref )
			{
				workCoord = cmd_info.cartref->cpos;
 			}

			client_command.EndPos.coordRef.X = workCoord.pos.x;
			client_command.EndPos.coordRef.Y = workCoord.pos.y;
			client_command.EndPos.coordRef.Z = workCoord.pos.z;
			client_command.EndPos.coordRef.A = rad2deg( workCoord.ang.x );
			client_command.EndPos.coordRef.B = rad2deg( workCoord.ang.y );
			client_command.EndPos.coordRef.C = rad2deg( workCoord.ang.z );
 		}
  
		if ( isHaveTool )
		{
 			if ( isRotateTcp )
			{
 			   //cout << " do rotate tcp motion use PTPRel.\n";
			   rotate_tcp_with_tool( cmd_info );
			   return;
			}
 		}
 	}

	robot_client.set_motion_finished(false);

	robot_client.putCommandInQueue(client_command);
}


int CodeSysMotion::rotate_tcp_with_tool( const CmdInfo& cmd_info )
{
	if ( robot_client.is_finished() == false )
	{
		return 0;
	}

	double arcLength = 10; //mm
	double arcRadius = 10;
	double arcAng = arcLength / arcRadius; // rad
	int nAxis = cmd_info.chJogAxisMark;
	int dir   = cmd_info.chJogDirMark;

	int nJogDir[MAX_JOINT_NUM] = {0};
	for (unsigned int i = 0; i < 8; i++ )
	{
		nJogDir[i] = ( ( 1 << i ) & dir ) ? -1 : 1;
	}

	int mdir = 1;
	switch( nAxis )
	{
	case ROTATE_X_AXIS: mdir = nJogDir[3]; break;
	case ROTATE_Y_AXIS: mdir = nJogDir[4]; break;
	case ROTATE_Z_AXIS: mdir = nJogDir[5]; break;
	default: break;
	}
  
	PosInfo cur_pos = get_last_axispos(); // get_position( PosInfo::CTmachine, LDScommad );
    std::vector<CartPos> robot_path;
	robot_path.clear();

	if ( cmd_info.iJogCoordType == jtTOOL ) // do RTCP motion base TCS 
	{
		const std::vector<CartPos>& axisend_list = pose_trans.get_rotate_tcp_path_emuler( 
			nAxis,
			mdir * arcLength,
			&cur_pos.cart, 
			NULL,   
			cmd_info.tool );	
		for ( unsigned int i = 0; i < axisend_list.size(); ++i )
		{
			robot_path.push_back(axisend_list[i]);
		}
	}
	else if ( cmd_info.iJogCoordType == jtWORLD ) // do RTCP motion base MCS 
	{
		CartRef world;
		const std::vector<CartPos>& axisend_list = pose_trans.get_rotate_tcp_path_emuler( 
			nAxis,
			mdir * arcLength,
			&cur_pos.cart, 
			&world,   
			cmd_info.tool );
		for ( unsigned int i = 0; i < axisend_list.size(); ++i )
		{
			robot_path.push_back(axisend_list[i]);
		}
	}
	else if ( cmd_info.iJogCoordType == jtUSER ) // do RTCP motion base PCS
	{
		const std::vector<CartPos>& axisend_list = pose_trans.get_rotate_tcp_path_emuler( 
			nAxis,
			mdir * arcLength,
			&cur_pos.cart, 
			cmd_info.cartref,   
			cmd_info.tool );	
		for ( unsigned int i = 0; i < axisend_list.size(); ++i )
		{
			robot_path.push_back(axisend_list[i]);
		}
	}

	Ysbot::ClientCommand newCmd = client_command;
	
	newCmd.CmdType = IEC_CMD_PTPABS;
	newCmd.VelFactor  = 1.0;
	newCmd.AccFactor  = 1.0;
	newCmd.JerkFactor = 1.0;
	newCmd.BufferMode = Buffered;
 	newCmd.TransMode  = TMNone;
 	newCmd.CoordSystem = MCS; // 0 : ACS, 1 : MCS, 2 : WCS,

	robot_client.set_motion_finished(false);
	for ( unsigned int i = 0 ; i < robot_path.size(); ++i )
	{
  		newCmd.EndPos.coordRef.X = robot_path[i].pos.x;
		newCmd.EndPos.coordRef.Y = robot_path[i].pos.y;
		newCmd.EndPos.coordRef.Z = robot_path[i].pos.z;
		newCmd.EndPos.coordRef.A = rad2deg(robot_path[i].ang.x );
		newCmd.EndPos.coordRef.B = rad2deg(robot_path[i].ang.y );
		newCmd.EndPos.coordRef.C = rad2deg(robot_path[i].ang.z );
        
		last_robot_pos.cart.set_pos(robot_path[i].pos.x,robot_path[i].pos.y,robot_path[i].pos.z,
			                        robot_path[i].ang.x,robot_path[i].ang.y,robot_path[i].ang.z );

		robot_client.putCommandInQueue(newCmd);
	}
   
 	return 0;
}

void CodeSysMotion::move_ptp(const CmdInfo& cmd_info)
{
	const double max_accdec_factor = 1.0;
 	client_command.CmdType = IEC_CMD_PTPABS;
	client_command.VelFactor  = cmd_info.dyn.JointVel * 0.01; // 30
	client_command.AccFactor  = min( max( cmd_info.dyn.JointAcc * 0.01, 0.0 ), max_accdec_factor ); // 3   
	client_command.JerkFactor = cmd_info.dyn.JointJerk * 0.01;
	client_command.BufferMode = cmd_info.ovl.iBufferMode;
	client_command.TransMode  = cmd_info.ovl.iTransMode;
	client_command.TransParam[0] = cmd_info.ovl.TransitionParameter[0];
	client_command.TransParam[1] = cmd_info.ovl.TransitionParameter[1];
	client_command.CoordSystem   = cmd_info.destPos.pos_type; // 0 : ACS, 1 : MCS, 2 : WCS,

	if ( cmd_info.dyn.PathVel == 0.0 )
 		client_command.iParams[0] = 1; // SMC_PTP_MOVEMENT_TYPE (ENUM) Fast = 0, Path_Invariant = 1 error:12130
 	else
		client_command.iParams[0] = 0;

    if ( cmd_info.destPos.pos_type == PosInfo::CTaxis )
    {
		client_command.EndPos.apos.a0 = rad2deg( cmd_info.destPos.axis.q1 );
		client_command.EndPos.apos.a1 = rad2deg( cmd_info.destPos.axis.q2 );
		client_command.EndPos.apos.a2 = rad2deg( cmd_info.destPos.axis.q3 );
		client_command.EndPos.apos.a3 = rad2deg( cmd_info.destPos.axis.q4 );
		client_command.EndPos.apos.a4 = rad2deg( cmd_info.destPos.axis.q5 );
		client_command.EndPos.apos.a5 = rad2deg( cmd_info.destPos.axis.q6 );
    }
	else if ( cmd_info.destPos.pos_type == PosInfo::CTproduct_2 )
	{
		CartPos destpi    = cmd_info.destPos.cart;
 		client_command.EndPos.coordRef.X = destpi.pos.x;
		client_command.EndPos.coordRef.Y = destpi.pos.y;
		client_command.EndPos.coordRef.Z = destpi.pos.z;
		client_command.EndPos.coordRef.A = rad2deg(destpi.ang.x );
		client_command.EndPos.coordRef.B = rad2deg(destpi.ang.y );
		client_command.EndPos.coordRef.C = rad2deg(destpi.ang.z );
	}
	else  
	{
		PosInfo destpi    = cmd_info.destPos;
		CartPos axisendpos;
		MWM.calc_axisend_pose( &destpi.cart, cmd_info.cartref, cmd_info.tool, &axisendpos ); // remove tool, sixth axis tip's pose base WCS.
  
		client_command.EndPos.coordRef.X = axisendpos.pos.x;
		client_command.EndPos.coordRef.Y = axisendpos.pos.y;
		client_command.EndPos.coordRef.Z = axisendpos.pos.z;
		client_command.EndPos.coordRef.A = rad2deg(axisendpos.ang.x );
		client_command.EndPos.coordRef.B = rad2deg(axisendpos.ang.y );
		client_command.EndPos.coordRef.C = rad2deg(axisendpos.ang.z );
	}

	robot_client.set_motion_finished(false);
	robot_client.putCommandInQueue(client_command);
}
 

void CodeSysMotion::move_line(const CmdInfo& cmd_info)
{
  	client_command.CmdType = IEC_CMD_LINABS;
	client_command.Velocity     = cmd_info.dyn.PathVel;
	client_command.Acceleration = cmd_info.dyn.PathAcc;
	client_command.Deceleration = cmd_info.dyn.PathDec;
	client_command.Jerk         = cmd_info.dyn.PathJerk;

	client_command.VelFactor    = 1.0;//cmd_info.dyn.JointVel * 0.01;
	client_command.AccFactor    = 1.0;//cmd_info.dyn.JointAcc * 0.01;
	client_command.JerkFactor   = 1.0;//cmd_info.dyn.JointJerk * 0.01;
	client_command.BufferMode   = cmd_info.ovl.iBufferMode;
	client_command.TransMode    = cmd_info.ovl.iTransMode;
	client_command.TransParam[0] = cmd_info.ovl.TransitionParameter[0];
	client_command.TransParam[1] = cmd_info.ovl.TransitionParameter[1];
	client_command.CoordSystem   = cmd_info.destPos.pos_type; // 0 : ACS, 1 : MCS, 2 : WCS, PCS_1, PCS_2, TCS
	client_command.OrientIPOMode = GreatCircle; // Axis GreatCircle

	if ( cmd_info.destPos.pos_type == PosInfo::CTaxis )
	{
		client_command.EndPos.apos.a0 = rad2deg( cmd_info.destPos.axis.q1 );
		client_command.EndPos.apos.a1 = rad2deg( cmd_info.destPos.axis.q2 );
		client_command.EndPos.apos.a2 = rad2deg( cmd_info.destPos.axis.q3 );
		client_command.EndPos.apos.a3 = rad2deg( cmd_info.destPos.axis.q4 );
		client_command.EndPos.apos.a4 = rad2deg( cmd_info.destPos.axis.q5 );
		client_command.EndPos.apos.a5 = rad2deg( cmd_info.destPos.axis.q6 );
	}
	else if ( cmd_info.destPos.pos_type == PosInfo::CTproduct_2 )
	{
		CartPos destpi    = cmd_info.destPos.cart;
		client_command.EndPos.coordRef.X = destpi.pos.x;
		client_command.EndPos.coordRef.Y = destpi.pos.y;
		client_command.EndPos.coordRef.Z = destpi.pos.z;
		client_command.EndPos.coordRef.A = rad2deg(destpi.ang.x );
		client_command.EndPos.coordRef.B = rad2deg(destpi.ang.y );
		client_command.EndPos.coordRef.C = rad2deg(destpi.ang.z );
	}
	else  
	{
		PosInfo destpi    = cmd_info.destPos;
		CartPos axisendpos;
		MWM.calc_axisend_pose( &destpi.cart, cmd_info.cartref, cmd_info.tool, &axisendpos ); // remove tool, sixth axis tip's pose base WCS.

		client_command.EndPos.coordRef.X = axisendpos.pos.x;
		client_command.EndPos.coordRef.Y = axisendpos.pos.y;
		client_command.EndPos.coordRef.Z = axisendpos.pos.z;
		client_command.EndPos.coordRef.A = rad2deg(axisendpos.ang.x );
		client_command.EndPos.coordRef.B = rad2deg(axisendpos.ang.y );
		client_command.EndPos.coordRef.C = rad2deg(axisendpos.ang.z );

// 		client_command.EndPos.coordRef.X = cmd_info.destPos.cart.pos.x;
// 		client_command.EndPos.coordRef.Y = cmd_info.destPos.cart.pos.y;
// 		client_command.EndPos.coordRef.Z = cmd_info.destPos.cart.pos.z;
// 		client_command.EndPos.coordRef.A = rad2deg(cmd_info.destPos.cart.ang.x );
// 		client_command.EndPos.coordRef.B = rad2deg(cmd_info.destPos.cart.ang.y );
// 		client_command.EndPos.coordRef.C = rad2deg(cmd_info.destPos.cart.ang.z );
	}

	robot_client.set_motion_finished(false);
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::move_circle(const CmdInfo& cmd_info)
{
 	client_command.CmdType        = IEC_CMD_CIRABS;
	client_command.CircMode       = BORDER;
	client_command.CircPathChoice = COUNTER_CLOCKWISE;
	client_command.OrientIPOMode  = GreatCircle;

	client_command.Velocity     = cmd_info.dyn.PathVel;
	client_command.Acceleration = cmd_info.dyn.PathAcc;
	client_command.Deceleration = cmd_info.dyn.PathDec;
	client_command.Jerk         = cmd_info.dyn.PathJerk;

	client_command.VelFactor  = 1.0;//cmd_info.dyn.JointVel * 0.01;
	client_command.AccFactor  = 1.0;//cmd_info.dyn.JointAcc * 0.01;
	client_command.JerkFactor = 1.0;//cmd_info.dyn.JointJerk * 0.01;

	client_command.BufferMode = cmd_info.ovl.iBufferMode;
	client_command.TransMode  = cmd_info.ovl.iTransMode;
	client_command.TransParam[0] = cmd_info.ovl.TransitionParameter[0];
	client_command.TransParam[1] = cmd_info.ovl.TransitionParameter[1];
	client_command.CoordSystem   = cmd_info.destPos.pos_type; // 0 : ACS, 1 : MCS, 2 : WCS,

	if ( cmd_info.destPos.pos_type == PosInfo::CTaxis )
	{
		client_command.CircAuxPos.apos.a0 = rad2deg( cmd_info.auxPos.axis.q1 );
		client_command.CircAuxPos.apos.a1 = rad2deg( cmd_info.auxPos.axis.q2 );
		client_command.CircAuxPos.apos.a2 = rad2deg( cmd_info.auxPos.axis.q3 );
		client_command.CircAuxPos.apos.a3 = rad2deg( cmd_info.auxPos.axis.q4 );
		client_command.CircAuxPos.apos.a4 = rad2deg( cmd_info.auxPos.axis.q5 );
		client_command.CircAuxPos.apos.a5 = rad2deg( cmd_info.auxPos.axis.q6 );
 
		client_command.EndPos.apos.a0 = rad2deg( cmd_info.destPos.axis.q1 );
		client_command.EndPos.apos.a1 = rad2deg( cmd_info.destPos.axis.q2 );
		client_command.EndPos.apos.a2 = rad2deg( cmd_info.destPos.axis.q3 );
		client_command.EndPos.apos.a3 = rad2deg( cmd_info.destPos.axis.q4 );
		client_command.EndPos.apos.a4 = rad2deg( cmd_info.destPos.axis.q5 );
		client_command.EndPos.apos.a5 = rad2deg( cmd_info.destPos.axis.q6 );
	}
	else  
	{
		PosInfo auxpi    = cmd_info.auxPos;
		CartPos aux_axis_endpos;
		MWM.calc_axisend_pose( &auxpi.cart, cmd_info.cartref, cmd_info.tool, &aux_axis_endpos ); // remove tool, sixth axis tip's pose base WCS.
 
		client_command.CircAuxPos.coordRef.X = aux_axis_endpos.pos.x;
		client_command.CircAuxPos.coordRef.Y = aux_axis_endpos.pos.y;
		client_command.CircAuxPos.coordRef.Z = aux_axis_endpos.pos.z;
		client_command.CircAuxPos.coordRef.A = rad2deg( aux_axis_endpos.ang.x );
		client_command.CircAuxPos.coordRef.B = rad2deg( aux_axis_endpos.ang.y );
		client_command.CircAuxPos.coordRef.C = rad2deg( aux_axis_endpos.ang.z );
		//////////////////////////////////////////////////////////////////////////
		PosInfo destpi    = cmd_info.destPos;
		CartPos axisendpos;
		MWM.calc_axisend_pose( &destpi.cart, cmd_info.cartref, cmd_info.tool, &axisendpos ); // remove tool, sixth axis tip's pose base WCS.

		client_command.EndPos.coordRef.X = axisendpos.pos.x;
		client_command.EndPos.coordRef.Y = axisendpos.pos.y;
		client_command.EndPos.coordRef.Z = axisendpos.pos.z;
		client_command.EndPos.coordRef.A = rad2deg(axisendpos.ang.x );
		client_command.EndPos.coordRef.B = rad2deg(axisendpos.ang.y );
		client_command.EndPos.coordRef.C = rad2deg(axisendpos.ang.z );

// 		client_command.EndPos.coordRef.X = cmd_info.destPos.cart.pos.x;
// 		client_command.EndPos.coordRef.Y = cmd_info.destPos.cart.pos.y;
// 		client_command.EndPos.coordRef.Z = cmd_info.destPos.cart.pos.z;
// 		client_command.EndPos.coordRef.A = rad2deg( cmd_info.destPos.cart.ang.x );
// 		client_command.EndPos.coordRef.B = rad2deg( cmd_info.destPos.cart.ang.y );
// 		client_command.EndPos.coordRef.C = rad2deg( cmd_info.destPos.cart.ang.z );
	}

	robot_client.set_motion_finished(false);
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::move_ptp_rel(const CmdInfo& cmd_info)
{
	const double max_accdec_factor = 1.0;
	client_command.CmdType = IEC_CMD_PTPREL;
	client_command.VelFactor  = cmd_info.dyn.JointVel * 0.01; // 30
	client_command.AccFactor  = min( max( cmd_info.dyn.JointAcc * 0.01, 0.0 ), max_accdec_factor ); // 3   
	client_command.JerkFactor = cmd_info.dyn.JointJerk * 0.01;
	client_command.BufferMode = cmd_info.ovl.iBufferMode;
	client_command.TransMode  = cmd_info.ovl.iTransMode;
	client_command.TransParam[0] = cmd_info.ovl.TransitionParameter[0];
	client_command.TransParam[1] = cmd_info.ovl.TransitionParameter[1];
	client_command.CoordSystem   = cmd_info.destPos.pos_type; // 0 : ACS, 1 : MCS, 2 : WCS,

	if ( cmd_info.dyn.PathVel == 0.0 )
		client_command.iParams[0] = 1; // SMC_PTP_MOVEMENT_TYPE (ENUM) Fast = 0, Path_Invariant = 1 
	else
		client_command.iParams[0] = 0;

	if ( cmd_info.destPos.pos_type == PosInfo::CTaxis )
	{
		client_command.RelDistance.apos.a0 = rad2deg(cmd_info.destPos.axis.q1);
		client_command.RelDistance.apos.a1 = rad2deg(cmd_info.destPos.axis.q2);
		client_command.RelDistance.apos.a2 = rad2deg(cmd_info.destPos.axis.q3);
		client_command.RelDistance.apos.a3 = rad2deg(cmd_info.destPos.axis.q4);
		client_command.RelDistance.apos.a4 = rad2deg(cmd_info.destPos.axis.q5);
		client_command.RelDistance.apos.a5 = rad2deg(cmd_info.destPos.axis.q6);
	}
	else  
	{
		client_command.RelDistance.coordRef.X = cmd_info.destPos.cart.pos.x;
		client_command.RelDistance.coordRef.Y = cmd_info.destPos.cart.pos.y;
		client_command.RelDistance.coordRef.Z = cmd_info.destPos.cart.pos.z;
		client_command.RelDistance.coordRef.A = rad2deg(cmd_info.destPos.cart.ang.x);
		client_command.RelDistance.coordRef.B = rad2deg(cmd_info.destPos.cart.ang.y);
		client_command.RelDistance.coordRef.C = rad2deg(cmd_info.destPos.cart.ang.z);

	}

	robot_client.set_motion_finished(false);
	robot_client.putCommandInQueue(client_command);
}

// program run use ACS and MCS, jog use ACS MCS WCS PCS_1 PCS_2 TCS.
void CodeSysMotion::move_line_rel(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_LINREL;
	client_command.Velocity     = cmd_info.dyn.PathVel;
	client_command.Acceleration = cmd_info.dyn.PathAcc;
	client_command.Deceleration = cmd_info.dyn.PathDec;
	client_command.Jerk         = cmd_info.dyn.PathJerk;

	client_command.VelFactor    = 1.0;//cmd_info.dyn.JointVel * 0.01;
	client_command.AccFactor    = 1.0;//cmd_info.dyn.JointAcc * 0.01;
	client_command.JerkFactor   = 1.0;//cmd_info.dyn.JointJerk * 0.01;
	client_command.BufferMode   = cmd_info.ovl.iBufferMode;
	client_command.TransMode    = cmd_info.ovl.iTransMode;
	client_command.TransParam[0] = cmd_info.ovl.TransitionParameter[0];
	client_command.TransParam[1] = cmd_info.ovl.TransitionParameter[1];
	client_command.CoordSystem   = 2;//cmd_info.destPos.pos_type; // 0 : ACS, 1 : MCS, 2 : WCS,
	client_command.OrientIPOMode = Axis;

	if ( cmd_info.destPos.pos_type == PosInfo::CTaxis )
	{
 		client_command.RelDistance.apos.a0 = rad2deg(cmd_info.destPos.axis.q1);
		client_command.RelDistance.apos.a1 = rad2deg(cmd_info.destPos.axis.q2);
		client_command.RelDistance.apos.a2 = rad2deg(cmd_info.destPos.axis.q3);
		client_command.RelDistance.apos.a3 = rad2deg(cmd_info.destPos.axis.q4);
		client_command.RelDistance.apos.a4 = rad2deg(cmd_info.destPos.axis.q5);
		client_command.RelDistance.apos.a5 = rad2deg(cmd_info.destPos.axis.q6);
 	}
	else  
	{
 		client_command.RelDistance.coordRef.X = cmd_info.destPos.cart.pos.x;
		client_command.RelDistance.coordRef.Y = cmd_info.destPos.cart.pos.y;
		client_command.RelDistance.coordRef.Z = cmd_info.destPos.cart.pos.z;
		client_command.RelDistance.coordRef.A = rad2deg(cmd_info.destPos.cart.ang.x);
		client_command.RelDistance.coordRef.B = rad2deg(cmd_info.destPos.cart.ang.y);
		client_command.RelDistance.coordRef.C = rad2deg(cmd_info.destPos.cart.ang.z);
 
	}

	robot_client.set_motion_finished(false);
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::move_circle_rel(const CmdInfo& cmd_info)
{
	client_command.CmdType        = IEC_CMD_CIRREL;
	client_command.CircMode       = BORDER;
	client_command.CircPathChoice = COUNTER_CLOCKWISE;
	client_command.OrientIPOMode  = GreatCircle;

	client_command.Velocity     = cmd_info.dyn.PathVel;
	client_command.Acceleration = cmd_info.dyn.PathAcc;
	client_command.Deceleration = cmd_info.dyn.PathDec;
	client_command.Jerk         = cmd_info.dyn.PathJerk;

	client_command.VelFactor  = 1.0;//cmd_info.dyn.JointVel * 0.01;
	client_command.AccFactor  = 1.0;//cmd_info.dyn.JointAcc * 0.01;
	client_command.JerkFactor = 1.0;//cmd_info.dyn.JointJerk * 0.01;

	client_command.BufferMode = cmd_info.ovl.iBufferMode;
	client_command.TransMode  = cmd_info.ovl.iTransMode;
	client_command.TransParam[0] = cmd_info.ovl.TransitionParameter[0];
	client_command.TransParam[1] = cmd_info.ovl.TransitionParameter[1];
	client_command.CoordSystem   = cmd_info.destPos.pos_type; // 0 : ACS, 1 : MCS, 2 : WCS,

	if ( cmd_info.destPos.pos_type == PosInfo::CTaxis )
	{
		client_command.CircAuxPos.apos.a0 = rad2deg( cmd_info.auxPos.axis.q1 );
		client_command.CircAuxPos.apos.a1 = rad2deg( cmd_info.auxPos.axis.q2 );
		client_command.CircAuxPos.apos.a2 = rad2deg( cmd_info.auxPos.axis.q3 );
		client_command.CircAuxPos.apos.a3 = rad2deg( cmd_info.auxPos.axis.q4 );
		client_command.CircAuxPos.apos.a4 = rad2deg( cmd_info.auxPos.axis.q5 );
		client_command.CircAuxPos.apos.a5 = rad2deg( cmd_info.auxPos.axis.q6 );

		client_command.EndPos.apos.a0 = rad2deg( cmd_info.destPos.axis.q1 );
		client_command.EndPos.apos.a1 = rad2deg( cmd_info.destPos.axis.q2 );
		client_command.EndPos.apos.a2 = rad2deg( cmd_info.destPos.axis.q3 );
		client_command.EndPos.apos.a3 = rad2deg( cmd_info.destPos.axis.q4 );
		client_command.EndPos.apos.a4 = rad2deg( cmd_info.destPos.axis.q5 );
		client_command.EndPos.apos.a5 = rad2deg( cmd_info.destPos.axis.q6 );
	}
	else  
	{
 		client_command.CircAuxPos.coordRef.X = cmd_info.auxPos.cart.pos.x;
		client_command.CircAuxPos.coordRef.Y = cmd_info.auxPos.cart.pos.y;
		client_command.CircAuxPos.coordRef.Z = cmd_info.auxPos.cart.pos.z;
		client_command.CircAuxPos.coordRef.A = rad2deg( cmd_info.auxPos.cart.ang.x );
		client_command.CircAuxPos.coordRef.B = rad2deg( cmd_info.auxPos.cart.ang.y );
		client_command.CircAuxPos.coordRef.C = rad2deg( cmd_info.auxPos.cart.ang.z );
		//////////////////////////////////////////////////////////////////////////
 
		client_command.EndPos.coordRef.X = cmd_info.destPos.cart.pos.x;
		client_command.EndPos.coordRef.Y = cmd_info.destPos.cart.pos.y;
		client_command.EndPos.coordRef.Z = cmd_info.destPos.cart.pos.z;
		client_command.EndPos.coordRef.A = rad2deg(cmd_info.destPos.cart.ang.x );
		client_command.EndPos.coordRef.B = rad2deg(cmd_info.destPos.cart.ang.y );
		client_command.EndPos.coordRef.C = rad2deg(cmd_info.destPos.cart.ang.z );
  
	}

	robot_client.set_motion_finished(false);
	robot_client.putCommandInQueue(client_command);
}
 
void CodeSysMotion::wait_input( const CmdInfo& cmd_info )
{
 	client_command.CmdType = IEC_CMD_WAITINPUT;
	client_command.nMaxElapsedTime = 0;
	int portid  = static_cast<int>(  static_cast<double>( cmd_info.iInOutNo )  / 16.0 ); // error
// 	int portid  = cmd_info.iInOutNo / 16;
	
	int bitid = cmd_info.iInOutNo % 16;
	client_command.wPortIndex = portid;
	client_command.iParams[0] = bitid; 
	client_command.wPortValue = cmd_info.iIoValue;
	robot_client.putCommandInQueue(client_command);

}

void CodeSysMotion::delay_motion( const CmdInfo& cmd_info ) 
{
	client_command.CmdType = IEC_CMD_DELAY;
	client_command.nMaxElapsedTime = 0;
	client_command.iParams[0] = cmd_info.iDelayTime; 
	client_command.iParams[1] = cmd_info.iObjNo;
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::set_digital_ouput(const CmdInfo& cmd_info)
{
 	client_command.CmdType = IEC_CMD_SETDIGITALOUTPUT;
	client_command.wPortIndex = cmd_info.iInOutNo;
	client_command.wPortValue = cmd_info.iIoValue;
 	robot_client.putCommandInQueue(client_command);
}

BOOL PeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))//
	{
		if (!AfxGetApp()->PumpMessage()) 
		{
			::PostQuitMessage(0);
			return FALSE;
		} 
	}
	return TRUE;
}
 
int do_events()
{
	BOOL ret = PeekAndPump();
	return (ret) ? 0 : -1;
}

int CodeSysMotion::wait_queue_notfull()
{
	while( robot_client.is_queue_full() && bAutoPlay )
	{
		if(	do_events() != 0 )
		{
			return -1;
		}
		Sleep(1);
	}
	return 0;
}

int CodeSysMotion::wait_finished()
{
	client_command.CmdType = IEC_CMD_WAITFINISHED;
	client_command.nMaxElapsedTime = 0;
	robot_client.putCommandInQueue(client_command);
 	return 0;
}

void CodeSysMotion::reset_group()
{
	bJogMotion = false;
	client_command.CmdType = IEC_CMD_RESETGROUP;
 	robot_client.putCommandInQueue(client_command);
}
 
void CodeSysMotion::hold_motion()
{
	client_command.CmdType = IEC_CMD_GROUPINTERRUPT;
	robot_client.putCommandInQueue(client_command);
	motion_info.iMotionStatus = MotionCoreInfo::msHOLD;
}


void CodeSysMotion::continue_motion()
{
	client_command.CmdType = IEC_CMD_GROUPCONTINUE;
	robot_client.putCommandInQueue(client_command);
	motion_info.iMotionStatus = MotionCoreInfo::msRUNNING;

}
 
void CodeSysMotion::abort_motion(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_GROUPHALT;
	client_command.Deceleration = 100;
	client_command.Jerk         = 1000;
	client_command.AccFactor    = 0.05;
	client_command.JerkFactor   = 1.0;
	robot_client.set_motion_finished(true);
 	robot_client.putCommandInQueue(client_command);
}

// dec acc 
void CodeSysMotion::stop_motion(const CmdInfo& cmd_info)
{
	bAutoPlay = false;
 	client_command.CmdType = IEC_CMD_GROUPSTOP;
	client_command.Deceleration = 1000;
	client_command.Jerk         = 2000;

	client_command.AccFactor    = 0.3;
	client_command.JerkFactor   = 1.0;

	bJogMotion = false;
 	robot_client.set_motion_finished(true);

	robot_client.putCommandInQueue(client_command);
	cout << "----------- stop motion ------ " << '\n';

}

void CodeSysMotion::stop_jog(const CmdInfo& cmd_info)
{
	cout << "----------- stop jog funtion ------ " << '\n';
	if ( bJogMotion )
	{
		bJogMotion = false;
		client_command.CmdType = IEC_CMD_STOPJOG;
		client_command.Deceleration = 1e4;
		client_command.Jerk         = 1e5;

		client_command.JerkFactor   = 1.0;
		client_command.AccFactor    = 1.0;
		client_command.VelFactor    = 1.0;

		client_command.iParams[0]   = cmd_info.iJogCoordType;
		client_command.nMaxElapsedTime = 1000;

		robot_client.set_motion_finished(true);
		robot_client.putCommandInQueue(client_command);
		cout << "----------- stop jog command ------ " << '\n';
	}
}

void CodeSysMotion::power_servo (const CmdInfo& cmd_info)
{
	if ( cmd_info.bOnOff )
	{
		client_command.CmdType = IEC_CMD_POWERON;
  	}
	else // 关
	{
		if ( robot_client.bHoldMotion )
		{
			stop_motion(cmd_info);
			while( robot_client.isAbortAllCommand ) // ????????????????????????
			{
				Sleep(1);
			}
		}

		client_command.CmdType = IEC_CMD_POWEROFF;
	}
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::set_position(const CmdInfo& cmd_info)
{
 	client_command.CmdType = IEC_CMD_SETPOS;
	if ( cmd_info.customStartPos )
	{
		client_command.EndPos.apos.a0 = rad2deg( cmd_info.startPos.axis.q1 );
		client_command.EndPos.apos.a1 = rad2deg( cmd_info.startPos.axis.q2 );
		client_command.EndPos.apos.a2 = rad2deg( cmd_info.startPos.axis.q3 );
		client_command.EndPos.apos.a3 = rad2deg( cmd_info.startPos.axis.q4 );
		client_command.EndPos.apos.a4 = rad2deg( cmd_info.startPos.axis.q5 );
		client_command.EndPos.apos.a5 = rad2deg( cmd_info.startPos.axis.q6 );

		client_command.Deceleration = 1e4;
		client_command.Jerk         = 1e5;

		client_command.JerkFactor   = 1.0;
		client_command.AccFactor    = 1.0;
		client_command.VelFactor    = 1.0;

		client_command.CoordSystem = ACS;
		robot_client.putCommandInQueue(client_command);

	}
 
}

const PosInfo& CodeSysMotion::get_position( const PosInfo::CoordinateType pct/* = mcACS */, const LocationDataSource lds/* = LDScommad */ ) throw(YsbotsException)
{
	robot_client.get_server_data(&server_data);

	pos_info.axis.set_deg(server_data.axisPos.apos.a0, server_data.axisPos.apos.a1, server_data.axisPos.apos.a2, 
		server_data.axisPos.apos.a3, server_data.axisPos.apos.a4, server_data.axisPos.apos.a5);

	SMC_POS_REF rpos = server_data.cartPosBaseMCS;
	pos_info.cart.set_pos( rpos.coordRef.X,rpos.coordRef.Y,rpos.coordRef.Z,
		deg2rad( rpos.coordRef.A ),
		deg2rad( rpos.coordRef.B ),
		deg2rad( rpos.coordRef.C ),
		5 );

	pos_info.pos_type = pct;

	return pos_info; 
}

const char*  CodeSysMotion::get_error()
{
	if ( server_data.errID > 0 )
	{
		std::string errstring = "error unknow code!";
		int i = 0;
		for( i = 0; i < ARRAY_SIZE(error_description); ++i )
		{
			if ( error_description[i].nErrorCode == server_data.errID )
			{
				errstring = error_description[i].lpszDesc;
		        return error_description[i].lpszDesc;
 			}
		}
		return errstring.c_str();
	}
	else
	{
		return 0;
	}
	
}

// thread call cycle.
const MotionCoreInfo& CodeSysMotion::get_motion_core_information()
{
	robot_client.get_server_data(&server_data);
	
	for ( int i = 0; i < 10; ++i )
	{
		motion_info.dfMemData[i] = server_data.dfMemData[i];
		motion_info.iMemData[i] = server_data.iMemdata[i];
	}
// 	robot_client.read_share_mem(&server_data_from_shm);
// 	server_data.group_state = server_data_from_shm.group_state;
	int cnt = 0;
	for ( int i = 0; i < server_data.iAxisNum; ++i )
	{
		if ( server_data.iServoAxisState[i] != Ysbot::power_off )
 			cnt++;
 	}

	if ( cnt == server_data.iAxisNum )
	{
		if ( MWM.get_robot_state().servo_on == false )
			MWM.set_servo_on(true);
	}
	else
	{
		if ( MWM.get_robot_state().servo_on == true )
			MWM.set_servo_on(false);
	}

	if ( server_data.group_state == Standby  )
	{
		motion_info.iMotionStatus = MotionCoreInfo::msSTOP;
		bool bJogMasterAxisMoving = false;
		for ( int i = 0; i < server_data.iAxisNum; ++i )
		{
			// jog master axis virtual.
			if ( server_data.iAxisState[i] != Ysbot::standstill && server_data.iAxisState[i] != Ysbot::errorstop &&
				 server_data.iAxisState[i] != Ysbot::power_off )
			{
				bJogMasterAxisMoving = true;
				break;
			}
		}
 
		if ( bJogMasterAxisMoving ) // consider master axis state when jog motion.
		{
			motion_info.iMotionStatus = MotionCoreInfo::msRUNNING;
 		}
 	}
	else if ( server_data.group_state == ErrorStop || server_data.group_state == Disabled  )
	{
		motion_info.iMotionStatus = MotionCoreInfo::msSTOP;
	}
	else if ( server_data.group_state == Moving || server_data.group_state == Homing || server_data.group_state == Stopping || 
		      robot_client.is_finished() == false )
	{
		motion_info.iMotionStatus = MotionCoreInfo::msRUNNING;
	}
    
	if ( server_data.iAuxGroupState == 2 )
	{
		motion_info.iMotionStatus = MotionCoreInfo::msHOLD;
 	}
	else if( server_data.iAuxGroupState == 3 )
	{ 
		motion_info.iMotionStatus = MotionCoreInfo::msDELAYING;
	}
 
	motion_info.nCommandIndex = server_data.iCurrMotionIndex;
	motion_info.emergency_stop = (server_data.group_state == ErrorStop) ? true : false;
	 
	if ( server_data.errID > 0 )
	{   
		char buffer[125] = {0}; 
	    int length = sprintf(buffer, "%d", server_data.errID); 
		string errorCode(buffer);
		std::string errstring = " CodeSys Motion Error code ";
		errstring = errstring + errorCode;

		for( int i = 0; i < ARRAY_SIZE(error_description); ++i )
		{
			if ( error_description[i].nErrorCode == server_data.errID )
			{
				errstring = error_description[i].lpszDesc;
				break;
			}
		}
		sprintf(motion_info.errString,"%s",errstring.c_str());

		bJogMotion = false;
	}
	else
	{
		motion_info.errString[0] = '\0';
	}

	return motion_info;
}

const int CodeSysMotion::get_group_status(unsigned int grpIdx /* = 0 */)
{
	return 0;
}

//Warning
//Reducing the AccFactor and/or JerkFactor can lead to a position overshoot, a possible cause of damage.

void CodeSysMotion::set_group_override(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_GROUPSETOVERRIDE;
	client_command.VelFactor = cmd_info.dyn.Override;
	client_command.AccFactor = 1.0;
	client_command.JerkFactor = 1.0;

	client_command.Velocity = cmd_info.dyn.Override;
	client_command.Acceleration = 1.0;
	client_command.Jerk = 1.0;

	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::set_last_axispos( const PosInfo& pi) throw()
{ 
	last_robot_pos = pi;  
}

const PosInfo& CodeSysMotion::get_last_axispos()  throw()
{
	if( robot_client.is_finished() ) 
	{ 
		last_robot_pos = get_position( PosInfo::CTmachine, LDScommad );
 	}
	else
	{
		cout << "use last pose\n";
	}

	return  last_robot_pos;
}

void CodeSysMotion::set_max_path_dyn()
{
	max_path_dyn.fVelMax = 2000;
	max_path_dyn.fAccMax = 6000;
	max_path_dyn.fDecMax = 6000;
	max_path_dyn.fJerkMax = 1000000.0;

	client_command.CmdType = IEC_CMD_SETMAXPATHSPEED;
	client_command.Velocity     = max_path_dyn.fVelMax;
	client_command.Acceleration = max_path_dyn.fAccMax;
	client_command.Deceleration = max_path_dyn.fDecMax;
	client_command.Jerk         = max_path_dyn.fJerkMax;
	client_command.nMaxElapsedTime = 0;
	robot_client.putCommandInQueue(client_command);
}
 
void CodeSysMotion::set_max_joint_dyn()
{
	double rpm[6] = { 3000, 3000, 3000, 3000, 3000, 3000 };
	double gear[6] = { 119, 119, 119, 100, 80, 50 };
	double factorManual[6] = { 2.0, 2.0, 2.5, 3.5, 3.75, 5.0 };
	//double factorAuto[6] = { 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
	double maxJerk[6] = { 2000.0, 2000.0, 2000.0, 6000.0, 5000.0, 7000.0 };

	for ( int i=0; i < 6; ++i )
	{
		max_joint_dyn[i].fVelMax  = rpm[i] * 360.0 / 60.0 / gear[i];
		max_joint_dyn[i].fAccMax  = max_joint_dyn[i].fDecMax = max_joint_dyn[i].fVelMax * factorManual[i];
		max_joint_dyn[i].fJerkMax = maxJerk[i];//max_joint_dyn[i].fVelMax * 10.0;
	}
  
	client_command.CmdType = IEC_CMD_SETMAXJOINTSPEED;
	double* pValArray[] = { &client_command.EndPos.valArray[0], 
 		                    &client_command.RelDistance.valArray[0],
 							&client_command.CircAuxPos.valArray[0],
						  };

	for ( int i =0; i < 3; ++i )
	{
		for ( int j = 0; j < 2; ++j )
		{
			*pValArray[i]++ = max_joint_dyn[j+i*2].fVelMax;
			*pValArray[i]++ = max_joint_dyn[j+i*2].fAccMax;
			*pValArray[i]++ = max_joint_dyn[j+i*2].fJerkMax;
		}
	}
/*
	client_command.EndPos.valArray[0] = max_joint_dyn[0].fVelMax;
	client_command.EndPos.valArray[1] = max_joint_dyn[0].fAccMax;
	client_command.EndPos.valArray[2] = max_joint_dyn[0].fJerkMax;

	client_command.EndPos.valArray[3] = max_joint_dyn[1].fVelMax;
	client_command.EndPos.valArray[4] = max_joint_dyn[1].fAccMax;
	client_command.EndPos.valArray[5] = max_joint_dyn[1].fJerkMax;

	client_command.RelDistance.valArray[0] = max_joint_dyn[2].fVelMax;
	client_command.RelDistance.valArray[1] = max_joint_dyn[2].fAccMax;
	client_command.RelDistance.valArray[2] = max_joint_dyn[2].fJerkMax;

	client_command.RelDistance.valArray[3] = max_joint_dyn[3].fVelMax;
	client_command.RelDistance.valArray[4] = max_joint_dyn[3].fAccMax;
	client_command.RelDistance.valArray[5] = max_joint_dyn[3].fJerkMax;

	client_command.CircAuxPos.valArray[0] = max_joint_dyn[4].fVelMax;
	client_command.CircAuxPos.valArray[1] = max_joint_dyn[4].fAccMax;
	client_command.CircAuxPos.valArray[2] = max_joint_dyn[4].fJerkMax;

	client_command.CircAuxPos.valArray[3] = max_joint_dyn[5].fVelMax;
	client_command.CircAuxPos.valArray[4] = max_joint_dyn[5].fAccMax;
	client_command.CircAuxPos.valArray[5] = max_joint_dyn[5].fJerkMax;
*/
	client_command.nMaxElapsedTime = 0;
	robot_client.putCommandInQueue(client_command);
}

void CodeSysMotion::conveyor_begin(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_CONVEYORBEGIN;
	client_command.Velocity     = cmd_info.dyn.PathVel;
	client_command.Acceleration = cmd_info.dyn.PathAcc;
	client_command.Deceleration = cmd_info.dyn.PathDec;
	client_command.Jerk         = cmd_info.dyn.PathJerk;
	// selects belt needed to track according to coordType.
	client_command.CoordSystem  = cmd_info.destPos.pos_type; // PCS_1, or PCS_2
	client_command.wPortValue = cmd_info.iIoValue;

	client_command.nMaxElapsedTime = 0;
	robot_client.putCommandInQueue(client_command);
}

int CodeSysMotion::conveyor_wait(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_CONVEYORWAIT;
	client_command.dfParams[0] = cmd_info.auxPos.cart.pos.x; //min work area
	client_command.dfParams[1] = cmd_info.auxPos.cart.pos.y; // max work area
	client_command.iParams[2] = cmd_info.iObjNo; // sensor type, may be laser, radar, photoelectric and so on.
 	client_command.CoordSystem = cmd_info.destPos.pos_type; // PCS_1, or PCS_2

	client_command.EndPos.coordRef.X = cmd_info.startPos.cart.pos.x;
	client_command.EndPos.coordRef.Y = cmd_info.startPos.cart.pos.y;
	client_command.EndPos.coordRef.Z = cmd_info.startPos.cart.pos.z;
	client_command.EndPos.coordRef.A = rad2deg(cmd_info.startPos.cart.ang.x);
	client_command.EndPos.coordRef.B = rad2deg(cmd_info.startPos.cart.ang.y);
	client_command.EndPos.coordRef.C = rad2deg(cmd_info.startPos.cart.ang.z);

	client_command.RelDistance.coordRef.X = cmd_info.destPos.cart.pos.x;
	client_command.RelDistance.coordRef.Y = cmd_info.destPos.cart.pos.y;
	client_command.RelDistance.coordRef.Z = cmd_info.destPos.cart.pos.z;
	client_command.RelDistance.coordRef.A = rad2deg(cmd_info.destPos.cart.ang.x);
	client_command.RelDistance.coordRef.B = rad2deg(cmd_info.destPos.cart.ang.y);
	client_command.RelDistance.coordRef.C = rad2deg(cmd_info.destPos.cart.ang.z);

	client_command.wPortValue = cmd_info.iIoValue;

	client_command.nMaxElapsedTime = 0;
 	robot_client.putCommandInQueue(client_command);
	return 0;
}

void CodeSysMotion::conveyor_done(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_CONVEYORDONE;
	client_command.CoordSystem = cmd_info.destPos.pos_type; // PCS_1, or PCS_2
	client_command.wPortValue = cmd_info.iIoValue;

  	robot_client.putCommandInQueue(client_command);
 
}

void CodeSysMotion::conveyor_end(const CmdInfo& cmd_info)
{
	client_command.CmdType = IEC_CMD_CONVEYOREND;
	client_command.CoordSystem = cmd_info.destPos.pos_type; // PCS_1, or PCS_2
	client_command.wPortValue = cmd_info.iIoValue;

	robot_client.putCommandInQueue(client_command);

}