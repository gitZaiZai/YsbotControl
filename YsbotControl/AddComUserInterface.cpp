// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "AddComUserInterface.h"
#include "Time.h"
#include "SerialPort.h"
#include "WorldModel.h"
#include "MotionControl.h"
#include "YsbotControlDlg.h"
#include "UIDialogs/state_dialog.h"

using namespace std;
/**---------------------------------------------------------------------- */
/*           7     6     5    4     3     2    1     0
	第1字节 3轴+  3轴-  2轴+ 2轴-  1轴+  1轴-  START STOP
	第2字节 F1    2nd   6轴+ 6轴-  5轴+  5轴-  4轴+  4轴-
	第3字节 V+    V-   Step  F/B   Jog   Rob   Mot   F2
	第4字节 备用  警报 坐标  程序  文件夹 变量 工具  U 
	第5字节 备用  备用 备用  备用  安全开关2   钥匙2
*/
/**---------------------------------------------------------------------- */

/**************  send data  *********************************************/
/* 第1字节 备用   备用 备用 备用 备用    备用   备用  备用
   第2字节 蜂鸣器 备用 备用 备用 Process Motion Error Run            */
/************************************************************************/


AddComUserInterface::AddComUserInterface(const ConfigReader& reader,  CSerialPort* tp) throw (InvalidConfigurationException):use_teachpendant(false),portnr(0),
	teach_pendant(tp)    
{
	TRACE(_T("Constuct AddComUserInterface!\n")); 

	for ( int i = 0; i < 5; i++ )
	{
 		revData[i] = 0;
		lastKeyData[i] = 0;
		isRiseSignal[i] = 0;
	}
 
 	if ( reader.get("TeachPendant::use_teach_pendant", use_teachpendant) <= 0 ) 
	{
		throw InvalidConfigurationException ("TeachPendant enable!");
	}
	if ( reader.get("TeachPendant::com_port", portnr) <= 0 ) 
	{
		throw InvalidConfigurationException ("TeachPendant com port");
	}
   
	if ( use_teachpendant )
	{
		CWnd *pWnd = AfxGetMainWnd();
		if ( pWnd != NULL )
		{	
			BOOL ret = teach_pendant->InitPort( pWnd, portnr, 38400, 'N', 8, 1 );
			if ( ret )
			{
				teach_pendant->StartMonitoring();
				set_command(true,LIGHTALL);
				lighttime.update();
			}
			else
			{
				throw InvalidConfigurationException ("TeachPendant open port failed!");
			}
		}
		else
			throw InvalidConfigurationException ("TeachPendant cwnd error!");
 		revtime.update();

	}

}

AddComUserInterface::~AddComUserInterface(void)
{
    set_command(false,LIGHTALL);
}
 
int AddComUserInterface::ProcessReceiveChar(unsigned char* chRX)
{
// 	char msg[255] = {'0'};
// 	sprintf( msg, "%02X %02X %02X %02X %02X \n", chRX[0], chRX[1], chRX[2], chRX[3], chRX[4]);
// 	cout << msg;
	 
	for ( int i = 0; i < 5; i++ )
		revData[i] = chRX[i];
 
	check_rise_signal3(revData,lastKeyData,isRiseSignal);

	return true;
}

void AddComUserInterface::set_command(bool b, DWORD sigMask)
{
	if( use_teachpendant && teach_pendant->get_flag_open() )
	{
 	    teach_pendant->set_command(b,sigMask);
	}
}

void AddComUserInterface::process_key_messages()
{
	getMainPageIndex();
	getShiftButton();
	getKeySwitch();

	getEnableSwitch();
	getVelocityAdjust();
    getMotKey();

	getRobKey();
// 	getF1RobButton(); // reset home position

	getStepKey();
	
	getJogCoordinate();

	getStartKey(); // 防止键盘线程被主线程的无限循环锁住

}

bool AddComUserInterface::process_messages()
{
	static bool offlight = false;
	
	if ( use_teachpendant )
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
		// 		teach_pendant->StopMonitoring();
		// 	}

		update_light_state();
	}
	else
	{
 		CSerialPort::jog_axis_control(revData);
		process_key_messages();  
// 		Sleep(30);
		for ( int k = 0; k < 4; ++k ) // reset key state from pc.
		{
		 	revData[k] = 0;
			lastKeyData[k] = 0;
		}
	}
  
// 	pMainDlg->right_dialog_.update_window();
// 	pMainDlg->state_dialog_.update_state_bar();
// 	pMainDlg->left_dialog_.update_window();

 	return true;
}
 
void AddComUserInterface::getEnableSwitch()
{
	bool enable = teach_pendant->enable_press;	 
	MWM.set_enable(enable);
}

void AddComUserInterface::getKeySwitch()
{
	OperateMachineT omt = OperateMachineT(CSerialPort::key_switch);//(revData[4] & 0X03)
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

void AddComUserInterface::getShiftButton()
{
 	MWM.set_in_second(CSerialPort::second_page);
}

void AddComUserInterface::getF1RobButton()
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

void AddComUserInterface::getStartKey()
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

void AddComUserInterface::getVelocityAdjust( )
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

void AddComUserInterface::getMotKey()
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
// 			MWM.set_servo_on(true);
// 			if ( MWM.get_robot_state().motion_light == lsOFF )
// 			{
// 				set_command(true,MOTIONSIG);
// 				MWM.set_motion_light(lsON);
// 			}
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
// 			MWM.set_servo_on(false);
			
		}
	}
}

void AddComUserInterface::getStepKey()
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

void AddComUserInterface::getMainPageIndex()
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
 
	MWM.set_window_index(ipage);
}

void AddComUserInterface::update_light_state()
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

void AddComUserInterface::getRobKey()
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

void AddComUserInterface::getJogCoordinate()
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

void AddComUserInterface::set_pc_keydata(unsigned int modifiers, unsigned int virtual_key)
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
	ProcessReceiveChar( keydatafrompc );
}

void AddComUserInterface::check_rise_signal( const int curr, int& last, bool& isrise )
{
	if ( curr == 1 )
	{
		if ( last == 0 )
		{
			last = 1;
			isrise = true;
		}
	}
	else
		last = curr;
}

void AddComUserInterface::check_rise_signal2( const unsigned char* curr, unsigned char* last, unsigned char* isrise )
{
	int bitstate = 0;
	int lastbit  = 0;
	for ( int i = 0; i < 5; ++ i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			bitstate = GETBIT(*(curr+i),j);
			if ( bitstate == 1 )
			{
				lastbit = GETBIT(*(last+i),j);
				if ( lastbit == 0 )
				{
					SETBIT(*(last+i),j);
					SETBIT(*(isrise+i),j);
				}
			}
			else
			{
				bitstate ? SETBIT(*(last+i),j) : CLRBIT(*(last+i),j);
			}
		}
	}

}

void AddComUserInterface::check_rise_signal3( const unsigned char* curr, unsigned char* last, unsigned char* isrise )
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