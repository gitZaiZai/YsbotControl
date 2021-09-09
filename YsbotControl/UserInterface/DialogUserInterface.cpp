// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

// DialogUserInterface.cpp : implementation file
//

#include "stdafx.h"
// #include "YsbotControl.h"
#include "DialogUserInterface.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../program_dialog.h"
#include "../Servo.h"
#include "../Journal.h"
#include "teachbox_tcp.h"
#include "../UIDialogs/state_dialog.h"

#define  DEBUG_TCP_TEACHPENDANT    (0)

BYTE  g_byPendantKeyTable[] = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', //  
	//  	VK_SHIFT ,
	// 	VK_CONTROL,
	// 	VK_MENU,
	0 // null-terminator
};

DialogUserInterface::DialogUserInterface( const ConfigReader& reader ) 
	: use_teachpendant(false), pendant_type(COM_SERIAL), com_port(0),enable_press(false),second_page(false),
	key_switch(0), techBoxOutput(0),the_simulator(NULL),enable_simulator(false),isResetKeyState(true)
{
	for ( int i = 0; i < 5; i++ )
	{
		revData[i] = 0;
		lastKeyData[i] = 0;
		isRiseSignal[i] = 0;
	}
 
 	string confline;
	if ( reader.get("TeachPendant::teach_pendat_type", confline)<=0) 
	{
		JERROR("no config line \"teach_pendat_type\" found");
 	}

	if  ( confline == "com_serial" )
	{
		if ( reader.get("TeachPendant::com_port", com_port) <= 0 )
		{
			//throw InvalidConfigurationException ("TeachPendant com port");
			MSD.ShowInfo(_T("TeachPendant com_port not found!"),1,1001);
		}

		BOOL ret = com_pendant.InitPort( this, com_port, 38400, 'N', 8, 1 );
		if ( ret )
		{
			com_pendant.StartMonitoring();
			set_command(true,LIGHTALL);
			lighttime.update();
		}
		else
		{
			// throw InvalidConfigurationException ("TeachPendant open port failed!");
			MSD.ShowInfo(_T("TeachPendant open port failed!"),1,1001);
		}
		pendant_type = COM_SERIAL;
	}
	else if ( confline == "tcp_socket" )
	{
 		if ( reader.get("TeachPendant::key_server_address", serveraddress) <= 0 ) 
		{
			//throw InvalidConfigurationException ("TeachPendant server address!");
			MSD.ShowInfo(_T("TeachPendant server address not found!"),1,1002);

		}
 		pendant_type = TCP_SOCKET;
 	}
	else if ( confline == "pc_keyboard" )
	{
		pendant_type = PC_KEYBOARD;
	}
	else
	{
		throw InvalidConfigurationException ("TeachPendant type!");
 	}

	if ( SMC.is_main_motion_control_available() )
	{
		SMC.set_pc_pendant ( (pendant_type == PC_KEYBOARD) ? true : false );
	}

	if ( MWM.is_main_world_model_available() )
	{
		MWM.set_vnc_desktop( pendant_type == TCP_SOCKET );
	}

 	revtime.update();

	the_simulator   = new SimulationPlatform(reader); 
}

DialogUserInterface::~DialogUserInterface()
{
	if ( pendant_type == TCP_SOCKET )
	{
		Ysbot::ExitTeachboxTcpThread();
	}

	if ( the_simulator )
	{
		delete the_simulator;
		the_simulator = NULL;
	}
}

void DialogUserInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogUserInterface, DialogType)
	ON_MESSAGE(WM_PENDANT_KEYPRESS,  &DialogUserInterface::OnTeachPendantKeyPress ) 
END_MESSAGE_MAP()


// DialogUserInterface message handlers

BOOL DialogUserInterface::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
 
	BOOL bret = FALSE;
	if ( pendant_type == TCP_SOCKET)
	{
 		bret = Ysbot::RunVncTeachboxTcp( this->GetSafeHwnd(), serveraddress.c_str() );
		if ( bret )
		{
			set_command(true,LIGHTALL);
			lighttime.update();
		}
  	}
	else if ( pendant_type == PC_KEYBOARD )
	{
		m_nVkFirst = 0;
		VERIFY( AfxGetMainWnd()->SendMessage( WM_ACQUIRE_KEY_INPUT,	(WPARAM) m_hWnd,(LPARAM) g_byPendantKeyTable) );
		timeKeyLastPressed.update();
	}
	return TRUE;
}

void DialogUserInterface::set_command( bool cmd, unsigned int sigMask )
{
	if ( pendant_type == COM_SERIAL )
	{
		if ( com_pendant.get_flag_open() )
		{
			com_pendant.set_command( cmd, sigMask );
		}
	}
	else if ( pendant_type == TCP_SOCKET )
	{
		if(cmd)
		{
			techBoxOutput |= sigMask;
 		}
		else
		{
			techBoxOutput &= ~sigMask;
 		}
		Ysbot::SetLedAndBuzzerData(techBoxOutput,0);
	}
}

bool DialogUserInterface::jog_axis( unsigned char* keyvalue )
{
	if ( GETBIT( keyvalue[0],0) ) // stop = 0
	{
		if ( program_dialog->GetSafeHwnd() )
		{
			program_dialog->on_free_script(true);
			//SMC.setCmdInfo(CMD_HALT); 
		}
		cout << " stop com key! " << '\n';
		return true;
	}

	unsigned char databyte = keyvalue[4] & 0X0C;
	if ( databyte == 4 )
		enable_press = true;
	else
		enable_press = false;

	// 0 remote; 1 play; 2 teach
	key_switch = keyvalue[4] & 0X03;

	unsigned char axis = 0;
	unsigned char dir  = 0;
	unsigned char databyte_first  = keyvalue[0];
	unsigned char databyte_second = keyvalue[1];

	unsigned char datashift = keyvalue[1] & 0X40;
	static int step = 0;
	if ( datashift != 0 )
	{
		step = 1;
	}
	else
	{
		if ( step == 1 )
		{
			second_page = !second_page;
			step = 0;
		}
	}

	if ( GETBIT(databyte_first,2) ^ GETBIT(databyte_first,3) )
	{
		int bpos = 0;
		if ( second_page )
			bpos = 6;
		SETBIT(axis,bpos);
		if (GETBIT(databyte_first,2))
			SETBIT(dir,bpos);
	}
	if ( GETBIT(databyte_first,4) ^ GETBIT(databyte_first,5) )
	{
		int bpos = 1;
		if ( second_page )
			bpos = 7;

		SETBIT(axis,bpos);
		if (GETBIT(databyte_first,4))
			SETBIT(dir,bpos);
	}
	if ( GETBIT(databyte_first,6) ^ GETBIT(databyte_first,7) && !second_page )
	{
		SETBIT(axis,2);
		if (GETBIT(databyte_first,6))
			SETBIT(dir,2);
	}

	if ( GETBIT(databyte_second,0) ^ GETBIT(databyte_second,1)  && !second_page )
	{
		SETBIT(axis,3);
		if (GETBIT(databyte_second,0))
			SETBIT(dir,3);
	}
	if ( GETBIT(databyte_second,2) ^ GETBIT(databyte_second,3)  && !second_page )
	{
		SETBIT(axis,4);
		if (GETBIT(databyte_second,2))
			SETBIT(dir,4);
	}
	if ( GETBIT(databyte_second,4) ^ GETBIT(databyte_second,5)  && !second_page )
	{
		SETBIT(axis,5);
		if (GETBIT(databyte_second,4))
			SETBIT(dir,5);
	}

	if ( key_switch == 2 ) // omTEACH
	{
		if ( axis != 0 )
		{
			if ( MWM.get_robot_state().servo_on == false )
			{
				SMC.setCmdInfo(CMD_ABORT); 
 				MSD.ShowInfo(_T("示教，设备未打开伺服使能！"),JTWARNING,0,true);

				return true;
			}

			if ( !enable_press )
			{
				SMC.setCmdInfo(CMD_ABORT); 
 				MSD.ShowInfo(_T(" 示教，请按下使能开关！"),JTWARNING,0,true);
 				return true;
			}

			CmdInfo cmd;

			if (MWM.get_robot_state().in_teachmoving )//位置示教允许中
			{
				if (axis != 1) 
				{
					return true;
				}
				if(dir)
					cmd.destPos = MWM.get_next_pos();//移向示教点
				else
					cmd.destPos = MWM.get_last_pos();//返回上一点
				if(MWM.get_teachmove_type() == 0)
					cmd.iCommandType =	CMD_LIN;
				else
					cmd.iCommandType = CMD_PTP;
				cmd.customStartPos = false;
				SMC.setCmdInfo(cmd);

				
				return true;
			}

			CartRef cartref = MWM.get_jog_cart().refSys;
			CartRef tool    = MWM.get_jog_tool().refSys;
			cmd.iCommandType  = CMD_JOG;
			cmd.iJogCoordType = MWM.get_robot_state().coordinate; // jtJOINT; 
			cmd.chJogAxisMark = axis;
			cmd.chJogDirMark  = dir;
			cmd.cartref = &cartref;
			cmd.tool    = &tool.cpos;
			// speed parameters
			cmd.dyn.custom_parameter = false;
			cmd.dyn.JointVel  = MWM.get_external_speed() > 10 ? 10 : MWM.get_external_speed();
			SMC.setCmdInfo( cmd );
			cout << "----------- jog axis control ------ " << '\n';
			return true;
		}
		else
		{
			// for check user teach program
			if ( ProgramEditor::is_running_flag == true ) 
			{
				if ( !enable_press )
				{
					if ( program_dialog->GetSafeHwnd() )
					{
						program_dialog->on_free_script(true);
					}					
 					MSD.ShowInfo(_T("试运行，请按下使能开关！"),JTWARNING,0,true);

					return true;
				}
			}
			else // the other is all stop
			{
				if ( MWM.get_robot_state().mc_info.iMotionStatus != MotionCoreInfo::msSTOP )
				{
					SMC.setCmdInfo(CMD_ABORT); 
				}
			}

			return false;
		}
	}
	else // remote or play
	{
		if ( axis != 0 )
		{
			MSD.ShowInfo(_T(" 示教，请切换到示教模式！"),JTWARNING,0,true);
 		}
	}
	return false;
}

LRESULT DialogUserInterface::OnTeachPendantKeyPress(WPARAM wp, LPARAM lp)
{
	if ( wp == 0 )
 		return 0;

	if ( pendant_type == COM_SERIAL || pendant_type == PC_KEYBOARD )
	{
		unsigned char* pData = (unsigned char*)(wp);
		for ( int i = 0; i < 5; i++ )
			revData[i] = pData[i];
 	}
	else if ( pendant_type == TCP_SOCKET )
	{
		unsigned int teachboxKeyValue0 = *(unsigned int*)(wp);
		unsigned int teachboxKeyValue1 = *(unsigned int*)(lp);
		unsigned char databuf[5] = {0};

		databuf[3] = static_cast<unsigned char>( (teachboxKeyValue0 & 0xff000000) >> 24 );     
		databuf[2] = static_cast<unsigned char>( (teachboxKeyValue0 & 0x00ff0000) >> 16 );
		databuf[1] = static_cast<unsigned char>( (teachboxKeyValue0 & 0x0000ff00) >> 8 );
		databuf[0] = static_cast<unsigned char>( teachboxKeyValue0 & 0x000000ff );
 		databuf[4] = static_cast<unsigned char>( teachboxKeyValue1 & 0x000000ff );

		unsigned char tcpdata[5] = {0};
		Ysbot::TcpToSerial( databuf, revData ); 
	}

#if DEBUG_TCP_TEACHPENDANT
	char msg[255] = {'0'};
	sprintf( msg, "%02X %02X %02X %02X %02X \n", revData[0], revData[1], revData[2], revData[3], revData[4]);
	std::cout << msg;
#endif


	if ( 0 )
	{
		if ( MWM.is_main_world_model_available()    == false ||
			SMC.is_main_motion_control_available() == false )
		{
			return 0;
		}

		if ( jog_axis( revData ) )
			return 1;

		check_rise_signal3(revData,lastKeyData,isRiseSignal);
	}
	else
	{
		if ( MWM.is_main_world_model_available() )
		{
			//datalock.Lock();
			MWM.set_key_list( KeyObjectList( revData, 5) ); 
			//datalock.Unlock();
			revtime.update();

			SMC.response_key_press(); // change to local function may be better.
			//process_key_messages();

			getStopKey();
		}
	}
  
 	return 1;
}

void DialogUserInterface::process_key_messages()
{
	const KeyObjectList& kol = MWM.get_key_list();
	for ( unsigned int i = 0; i < kol.keylist.size(); ++i )
	{
		revData[i] = kol.keylist[i];
	}
	check_rise_signal3(revData,lastKeyData,isRiseSignal);

	getMainPageIndex();
//	getShiftButton();
	getKeySwitch();

//	getEnableSwitch();
	getVelocityAdjust();
	getMotKey();

	getRobKey();
	// 	getF1RobButton(); // reset home position

	getStepKey();

	getJogCoordinate();

	getStartKey(); // 防止键盘线程被主线程的无限循环锁住

}

// on timer call.
bool DialogUserInterface::process_messages()
{
	static bool offlight = false;

	if ( pendant_type == COM_SERIAL || pendant_type == TCP_SOCKET)
	{
		process_key_messages();  
		if ( !offlight && lighttime.elapsed_msec() > 500 )
		{
			offlight = true;
			set_command(false,LIGHTALL);
			//  		getKeySwitch(); // update ui information with system startup.
		}

		// 	if( revtime.elapsed_msec() > 350 )
		// 	{
		// 		cout << "com port disconnect! \n";
		// 		StopMonitoring();
		// 	}

		update_light_state();
	}
 	else if( pendant_type == PC_KEYBOARD )
	{
		if ( isResetKeyState == false && timeKeyLastPressed.elapsed_msec() > 500 )
		{
			isResetKeyState = true;
			clear_key_value();
			m_nVkFirst = 0;
			cout << "manual reset \n";
		}
	}

	if ( enable_simulator  && the_simulator )
	{
		PosInfo pdq = SMC.get_position();
		AxisPos ap = pdq.axis.get_deg(); 
		double dfjnt[] = { ap.q1, ap.q2, ap.q3, ap.q4, ap.q5, ap.q6 };
		the_simulator->set_joint_data(dfjnt);
	}

	return true;
}

void DialogUserInterface::getEnableSwitch()
{
	bool enable = enable_press;	 
	MWM.set_enable(enable);
}

void DialogUserInterface::getKeySwitch()
{
	//OperateMachineT omt = OperateMachineT(key_switch);//(revData[4] & 0X03)
	OperateMachineT omt = OperateMachineT(SMC.key_switch);//(revData[4] & 0X03)

	if (MWM.is_main_world_model_available())
	{
		OperateMachineT old = MWM.get_robot_state().operate;
		if ( old != omt )
		{
			if ( ProgramEditor::is_running_flag )
			{
				program_dialog->on_free_script();
				if ( MWM.get_robot_state().run_light == lsON )
				{
					set_command( false, RUNSIG );
					MWM.set_run_light(lsOFF);
				}
			}
		}
		MWM.set_operation_mode(omt);
	}
}

void DialogUserInterface::getShiftButton()
{
	MWM.set_in_second(second_page);
}

void DialogUserInterface::getF1RobButton()
{
	unsigned char f1key  = static_cast<unsigned char> ( revData[1] );
	unsigned char robkey = static_cast<unsigned char> ( revData[2] );

	static int step = 0;
	if ( GETBIT(robkey,2) & GETBIT(f1key,7)  )
	{
		if ( step == 0 ) // wait operator finished per one times.
			step = 1;
	}
	else
	{
		if ( step == 1 )
		{
			// 			pMainDlg->state_dialog_.ShowInfo(_T(" 开始重置零点！"));
			MSD.ShowInfo(_T(" 开始重置零点！"),JTMESSAGE);
			cout << "------- clear servo pos -------" << '\n';
			if ( Servo::is_main_servo_available() )
			{
				int ret = Servo::get_main_servo().clear_all_servo_pos();
				if ( ret == 0 )
				{
					MSD.ShowInfo(_T(" 重置零点成功！"),JTMESSAGE);
				}
				else
				{
					cout << "----- clear pos failed -----------" << '\n';
					MSD.ShowInfo(_T(" 重置零点失败！"),JTMESSAGE);
				}
			}
			step = 0;
		}
	}
}

void DialogUserInterface::getStartKey()
{
	if ( GETBIT( isRiseSignal[0], 1 ) )
	{
		CLRBIT( isRiseSignal[0], 1 );

		if ( MWM.get_robot_state().operate == omTEACH && MWM.get_robot_state().enable_press == false )
		{  
			unsigned char databyte = static_cast<unsigned char> ( revData[0] ); // ???????????????????
			return;
		}
		if ( MWM.get_robot_state().program_run == prSTEP )
		{
			program_dialog->on_step_run_script();
		}
		else 
		{
			program_dialog->on_run_script();
		}
	}
}

void DialogUserInterface::getStopKey()
{
	const KeyObjectList& kol = MWM.get_key_list();
	if ( GETBIT( kol.keylist[0],0) )
	{
		clear_key_value();
	}
}

void DialogUserInterface::getVelocityAdjust( )
{
	unsigned char databyte = static_cast<unsigned char> ( revData[2] );
	if ( GETBIT(databyte,6) ^ GETBIT(databyte,7) )
	{
		double curr_speed = MWM.get_external_speed();
		double next_speed = 0;
		double maxstep = 1.0;
		double minstep = 0.05;
		if ( GETBIT(databyte,6) ) // minus
		{
			if ( curr_speed > 0 )
			{
				if( curr_speed <= 1 ) curr_speed -= minstep;
				else     			  curr_speed -= maxstep;
			}
		}
		else if( GETBIT(databyte,7) ) // plus
		{
			if ( curr_speed < 100 )
			{
				if( curr_speed >= 1 ) curr_speed += maxstep;
				else				  curr_speed += minstep;
			}
		}
		next_speed = ( curr_speed < 0.01 ) ? 0.01 : ( (curr_speed > 100) ? 100 : curr_speed );
		MWM.set_external_speed(next_speed);

		cout << "external_speed: " << MWM.get_external_speed() << '\n';
	}
}

// void DialogUserInterface::getMotKey()
// {
// 	if ( GETBIT( isRiseSignal[2], 1 ) )
// 	{
// 		CLRBIT( isRiseSignal[2], 1 );
// 
// 		CmdInfo cmd;
// 		cmd.iCommandType  = CMD_POWER;
// 		if ( MWM.get_robot_state().servo_on == false )
// 		{
// 			cmd.bOnOff = true;  
// 			SMC.setCmdInfo( cmd );
// 			MWM.set_servo_on(true);
// 			if ( MWM.get_robot_state().motion_light == lsOFF )
// 			{
// 				set_command(true,MOTIONSIG);
// 				MWM.set_motion_light(lsON);
// 			}
// 			cout << "on lingt1" << '\n';
// 		}
// 		else
// 		{
// 			if ( ProgramEditor::is_running_flag )
// 			{
// 				program_dialog->on_free_script();
// 				if ( MWM.get_robot_state().run_light == lsON )
// 				{
// 					set_command( false, RUNSIG );
// 					MWM.set_run_light(lsOFF);
// 				}
// 			}
// 
// 			cmd.bOnOff = false;  
// 			SMC.setCmdInfo( cmd );
// 			MWM.set_servo_on(false);
// 			if ( MWM.get_robot_state().motion_light == lsON )
// 			{
// 				set_command(false,MOTIONSIG);
// 				MWM.set_motion_light(lsOFF);
// 			}
// 
// 			cout << "off lingt2" << '\n';
// 		}
// 	}
// }


void DialogUserInterface::getMotKey()
{
	if ( SMC.isCodeSysMotion )
	{
		if ( MWM.get_robot_state().servo_on == true )
		{
			if ( MWM.get_robot_state().motion_light == lsOFF )
			{
				set_command(true,MOTIONSIG);
				MWM.set_motion_light(lsON);
				cout << "on lingt1" << '\n';
			}
		}
		else
		{
			if ( MWM.get_robot_state().motion_light == lsON )
			{
				set_command(false,MOTIONSIG);
				MWM.set_motion_light(lsOFF);
				cout << "off lingt2" << '\n';
			}

		}

		if ( GETBIT( isRiseSignal[2], 1 ) )
		{
			CLRBIT( isRiseSignal[2], 1 );

			CmdInfo cmd;
			cmd.iCommandType  = CMD_POWER;
			if ( MWM.get_robot_state().servo_on == false )
			{
				cmd.bOnOff = true;  
				SMC.setCmdInfo( cmd );
			}
			else // off
			{
				if ( ProgramEditor::is_running_flag )
				{
					MSD.ShowInfo(_T("请先停止程序运行！"),2);
					// program_dialog->on_free_script(); // abort after all command.
					return;
				}

				cmd.bOnOff = false;  
				SMC.setCmdInfo( cmd );
			}
		}
	}
	else // PMC MCCL motion
	{
		if ( GETBIT( isRiseSignal[2], 1 ) )
		{
			CLRBIT( isRiseSignal[2], 1 );

			CmdInfo cmd;
			cmd.iCommandType  = CMD_POWER;
			if ( MWM.get_robot_state().servo_on == false )
			{
				cmd.bOnOff = true;  
				SMC.setCmdInfo( cmd );
				MWM.set_servo_on(true);
				if ( MWM.get_robot_state().motion_light == lsOFF )
				{
					set_command(true,MOTIONSIG);
					MWM.set_motion_light(lsON);
				}
				cout << "on lingt1" << '\n';
			}
			else
			{
				if ( ProgramEditor::is_running_flag )
				{
					program_dialog->on_free_script();
					if ( MWM.get_robot_state().run_light == lsON )
					{
						set_command( false, RUNSIG );
						MWM.set_run_light(lsOFF);
					}
				}

				cmd.bOnOff = false;  
				SMC.setCmdInfo( cmd );
				MWM.set_servo_on(false);
				if ( MWM.get_robot_state().motion_light == lsON )
				{
					set_command(false,MOTIONSIG);
					MWM.set_motion_light(lsOFF);
				}

				cout << "off lingt2" << '\n';
			}
		}
	}

}

void DialogUserInterface::getStepKey()
{
	if ( GETBIT( isRiseSignal[2], 5 ) )
	{
		CLRBIT( isRiseSignal[2], 5 );
		ProgramRunT prt = MWM.get_robot_state().program_run;
		ProgramRunT newprt;
		switch(prt)
		{
		case prAUTO: newprt = prSTEP; break;
		case prSTEP: newprt = prAUTO; break;
		default: break;
		}
		MWM.set_programrun_mode(newprt);
	}
}

void DialogUserInterface::getMainPageIndex()
{
	int ipage = -1;
	for ( int i = 0; i < 7; i++ )
	{
		if ( GETBIT( isRiseSignal[3], i ) )
		{
			CLRBIT( isRiseSignal[3], i );
			ipage = i;
			break;
		}
	}

// 	if ( ipage == 0 || ipage == 1 )
// 	{
// 		GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winMainMenu,ipage);
// 	}
// 	else
// 	   MWM.set_window_index(ipage);

// 	GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winMainMenu,ipage);
	if ( ipage >= 0 && ipage < 7 )
	{
		GetParent()->SendMessage(WM_LEFT_KEY_PRESS,(UINT)ipage,0);
 	}
	MWM.set_window_index(ipage);
}

void DialogUserInterface::update_light_state()
{
	if ( MWM.get_robot_state().mc_info.iMotionStatus == MotionCoreInfo::msRUNNING )
	{
		if ( MWM.get_robot_state().run_light == lsOFF )
		{
			set_command(true,RUNSIG);
			MWM.set_run_light(lsON);
		}
	}
	else
	{
		if ( MWM.get_robot_state().run_light == lsON )
		{
			set_command(false,RUNSIG);
			MWM.set_run_light(lsOFF);
		}
	}
}

void DialogUserInterface::getRobKey()
{
	if ( GETBIT( isRiseSignal[2], 2 ) )
	{
		CLRBIT( isRiseSignal[2], 2 );

		unsigned char f1key  = static_cast<unsigned char> ( revData[1] );

		if ( GETBIT(f1key, 7) ) // F1+Rob
		{
			cout << " F1 + Rob pressed! \n";
			MSD.ShowInfo(_T(" 开始重置零点！"),JTMESSAGE);
			cout << "------- clear servo pos -------" << '\n';
			if ( Servo::is_main_servo_available() )
			{
				int ret = Servo::get_main_servo().clear_all_servo_pos();
				if ( ret == 0 )
				{
					MSD.ShowInfo(_T(" 重置零点成功！"),JTMESSAGE);
				}
				else
				{
					cout << "----- clear pos failed -----------" << '\n';
					MSD.ShowInfo(_T(" 重置零点失败！"),JTMESSAGE);
				}
			}
		}
		else
		{
			cout << "Rob pressed! reset motion. \n";

			CmdInfo cmd;
			cmd.iCommandType = CMD_RESET;
			SMC.setCmdInfo(cmd);
		}
	}
}

void DialogUserInterface::getJogCoordinate()
{
	if ( GETBIT( isRiseSignal[2], 3 ) )
	{
		CLRBIT( isRiseSignal[2], 3 );
		if ( MWM.get_robot_state().in_second == false )
		{
			JogCoordT jct = MWM.get_robot_state().coordinate;
			JogCoordT newcoord;
			switch(jct)
			{
			case jtJOINT: newcoord = jtWORLD; break;
			case jtWORLD: newcoord = jtTOOL;  break;
			case jtTOOL:  newcoord = jtUSER;  break;
			case jtUSER:  newcoord = jtJOINT; break;
			default: break;
			}
			MWM.set_jog_coordinate(newcoord);
		}
	}
}

void DialogUserInterface::check_rise_signal3( const unsigned char* curr, unsigned char* last, unsigned char* isrise )
{
	for ( int i = 0; i < 5; ++ i )
	{
		if ( *(curr+i) != 0 )
		{
			if ( *(last+i) == 0 )
			{
				*(isrise+i) = *(curr+i) ^ *(last+i);
				*(last+i)   = *(curr+i);
			}
		}
		else
			*(last+i) = *(curr+i);
	}

}

void DialogUserInterface::set_pc_keydata(unsigned int modifiers, unsigned int virtual_key)
{	
	static unsigned int switch_key_state = 0;
	unsigned char keydatafrompc[5] = {0};
	if ( MOD_SHIFT == modifiers ) // left key
	{
		SETBIT(keydatafrompc[3],unsigned char(virtual_key - '0'));
	}
	else if ( MOD_CONTROL == modifiers )
	{
		if (unsigned char(virtual_key) < '8' ) // bottom key
		{
			SETBIT(keydatafrompc[2],unsigned char(virtual_key - '0'));
		}
		else
		{
			if ( unsigned char(virtual_key) == '8') // key switch
			{
				if ( ++switch_key_state > 2 )	switch_key_state = 0;
			}
		}
	}
	else if( MOD_ALT == modifiers ) // right up key panel control function.
	{
		SETBIT(keydatafrompc[0],unsigned char(virtual_key - '0'));
	}
	else if( (MOD_CONTROL|MOD_ALT) == modifiers ) // right bottom key
	{
		SETBIT(keydatafrompc[1],unsigned char(virtual_key - '0'));
		keydatafrompc[1] = (keydatafrompc[1] >> 2) | (keydatafrompc[1] << 6); // 1 0 7 6 5 4 3 2
	}

	keydatafrompc[4] = switch_key_state | 4; // enable switch is not used for PC keyboard .

	//OnTeachPendantKeyPress((WPARAM) keydatafrompc, 0 );

	if ( MWM.is_main_world_model_available() )
	{
		//datalock.Lock();
		MWM.set_key_list( KeyObjectList( keydatafrompc, 5) ); 
		//datalock.Unlock();
		revtime.update();

		SMC.response_key_press();
		process_key_messages();

		getStopKey();
	}

}

void DialogUserInterface::toggle_second_page()
{
	SMC.second_page = !SMC.second_page;
}

void DialogUserInterface::clear_key_value()
{
	if ( pendant_type == PC_KEYBOARD )
	{
		for ( int k = 0; k < 4; ++k ) // reset key state from pc.
		{
			revData[k]     = 0;
			lastKeyData[k] = 0;
		}

		if ( MWM.is_main_world_model_available() )
		{
			//datalock.Lock();
			MWM.set_key_list( KeyObjectList( revData, 5) ); 
			//datalock.Unlock();
			revtime.update();
		}
	}
}

BOOL DialogUserInterface::PreTranslateMessage(MSG* pMsg)
{
	if( pendant_type == PC_KEYBOARD )
	{
		if( pMsg->message == WM_KEYDOWN    || pMsg->message == WM_KEYUP    || 
			pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP )
		{
			for(int i = 0; g_byPendantKeyTable[i]; ++i)
			{
				if( g_byPendantKeyTable[i] == (BYTE) pMsg->wParam ||
					( (::GetKeyState(VK_MENU) > 0) || (::GetKeyState(VK_SHIFT) > 0) || (::GetKeyState(VK_CONTROL) > 0) ))
				{
					OnKeyboardInput(
						(pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP),
						(int) pMsg->wParam);
					return TRUE;
				}
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void DialogUserInterface::OnKeyboardInput(BOOL bUpAndDown, int nVirtualKey)
{
	if( bUpAndDown ) // Key up
	{
		cout << "key up \n";
		clear_key_value();
		isResetKeyState = true;
		bool stop_jog_motion = false;
		if ( (::GetKeyState(VK_MENU) > 0) || (::GetKeyState(VK_SHIFT) > 0) || (::GetKeyState(VK_CONTROL) > 0) )  
		{
			stop_jog_motion = true;
		}

		if( nVirtualKey == m_nVkFirst )
		{

			switch( nVirtualKey )
			{
			case '2': case '3': case '4':
			case '5': case '6': case '7':
				stop_jog_motion = true;
				break;
			}
			m_nVkFirst = 0;
		}
		////??????????????????????

		if ( MWM.get_robot_state().operate == omTEACH && stop_jog_motion )
		{
			if ( SMC.isCodeSysMotion )
			{
				SMC.setCmdInfo(CMD_ABORTJOG); 
				cout << " send abort jog command to codesys.\n";
 			}
			else
				SMC.setCmdInfo(CMD_ABORT);  
			//SMC.response_key_press();
		} 
	}
	else if( m_nVkFirst == 0 || m_nVkFirst == nVirtualKey ) // Key down
	{
		isResetKeyState = false;
		timeKeyLastPressed.update();

		UINT modifiers = 0;
		if ( ::GetKeyState(VK_SHIFT) < 0 )
		{
			modifiers = MOD_SHIFT;
		}
		if ( ::GetKeyState(VK_CONTROL) < 0 )
		{
			modifiers |= MOD_CONTROL;
		}
		if ( ::GetKeyState(VK_MENU) < 0 )
		{
			modifiers |= MOD_ALT;
		}
		UINT virtual_key = nVirtualKey;
		//cout << "key press " << modifiers << " : " << virtual_key << '\n';

		switch( nVirtualKey )
		{
		case VK_UP: // 无需等待按键弹起，直接不断响应
			break;

		case VK_DOWN:
			break;

		case VK_ADD:      
			break;

		case VK_SUBTRACT: 
			break;

		default:
// 			if( m_nVkFirst == 0 ) // 弹起后才能接着响应 liurundan
			if( m_nVkFirst == 0  ) //|| m_nVkFirst == nVirtualKey 弹起后才能接着响应 
			{
				// 调用程序界面
				// 				if ( MOD_CONTROL == modifiers && 'D' == virtual_key) 
				// 				{
				// 					;
				// 				}
				if ( MOD_SHIFT == modifiers && '8' == virtual_key ) 
				{
					if ( the_simulator )
					{
						the_simulator->startup_simulator();
 					}
					enable_simulator = true;
					nVirtualKey = 0; // after shellexecute RoboDK, lead to key up message loss, so need reset key value manually. 
				}
				if ( MOD_SHIFT == modifiers && '9' == virtual_key )
				{
					enable_simulator = false;
					if ( the_simulator )
					{
						the_simulator->close_simulator();
 					}
					nVirtualKey = 0;
				}
				//cout << "Press [" << modifiers << "] : " << virtual_key << '\n';
				set_pc_keydata( modifiers, virtual_key );
			}
			break;
		}

		m_nVkFirst = nVirtualKey;
	}
}