// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"

#include "ScriptFunc.h"
 
#include "../MotionControl.h"
#include "../WorldModel.h"
#include "../FieldBusDevice.h"
#include "../MotionCoreInfo.h"
#include "../stringconvert.h"
#include "LuaScript.h"
#include "../Trigger.h"
#include "../RobotKinematics.h"
#include "../IO Dialog/InOutDialog.h"
#include "../SQLiteDB.h"
#include "../program_editor.h"
#include "../program_dialog.h"
#include "../UIDialogs/state_dialog.h"
#include "../IO Dialog/SettingIODlg.h"
#include "ScriptVariables.h"

namespace ScriptFunc
{

long edit_line_no       = -1;
unsigned long long last_command_index = 0;
PosInfo start_pos;
PosInfo end_pos;

CmdInfo cmd;
int lastConveyorWaitFlag[2] = {0,0};
int iConveyorCoordType = 0;

void init_script_function()
{
	edit_line_no       = -1;
	last_command_index = 0;
	lastConveyorWaitFlag[0] = lastConveyorWaitFlag[1] = 0; 
	start_pos = SMC.get_position();
	end_pos   = start_pos;
}

BOOL PeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
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
// 	static MSG msg;
//  	if (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) 
// 	{ 
// 		if ( msg.message == WM_QUIT ) 
// 			return -1; 
// 		::TranslateMessage (&msg) ; 
// 		::DispatchMessage (&msg) ; 
// 	} 
// 	return 0;
	BOOL ret = PeekAndPump();
  	return (ret) ? 0 : -1;
}

// 钩子函数在执行lua指令前触发
int luaHook(  const char* src, const char* short_src, int lineid, lua_State *L )
{
// 	int lineno = (int)lua_tonumber(L,1);
	int lineno = lineid;
// 函数定义的位置，函数是loadstring在一个字符串中定义的，那么source就是这个字符串。如果函数是在一个文件中定义的，那么source就是这个文件名加前缀‘@’。
	std::string sourceText = std::string(src); 
	std::string currText = std::string(short_src);
	bool bExternalModule = false;
	if ( sourceText.find_first_of('@') != std::string::npos || sourceText.find("package") != string::npos ||
		sourceText.find("require") != string::npos )
	{
		bExternalModule = true;
 	}

// 	cout << "lua execute : " << lineno << " [ " << string(src) << " ] " << " [ " << string(short_src) << " ] " <<  endl;
	cout << "lua execute : " << lineno <<  endl;
	if ( !bExternalModule )
	{
		edit_line_no = lineno; 
	}
 
	if ( ProgramEditor::is_running_flag )
	{
// 		program_dialog->editor_.HighlightLine(edit_line_no-1);
	}
	else
		return lua_yield(L,0);

    if ( do_events() != 0 )
    {
		return lua_yield(L,0); // 交出控制权 
    }

 	if ( ProgramEditor::line_hook_call( lineno, bExternalModule, src ) != 0 )
	{
		return lua_yield(L,0); // 交出控制权
 	}
	Sleep(1);
	return 0;
}

bool is_command_ignored()
{
	if ( program_dialog )
	{
		long _startLineNo = program_dialog->editor_.iStartLineNo;
		if ( _startLineNo != -1 && edit_line_no != -1 )
		{
			if ( edit_line_no <= _startLineNo )
			{
				return true;
			}
			else
			{
		        program_dialog->editor_.iStartLineNo = -1; // reset
			}
		}
	}

	return false;
}

// void ADD_PROG_COMMAND(int lineno, int ci)		
// {
// 	if( ci >= 0 )
// 	{
// 		ProgCommandInfo g_pci = MWM.get_prog_command_info();
// 
// 		unsigned int index = g_pci.nCmdCount;
// 
// 		// avoid runtime-51 error, buffer overwrite.
// 		if ( index == MAX_PROG_COMMAND-1 )
// 		{
// 			if ( isEmptyCommandBuffer() ) // wait motion finished
// 			{
// 				CmdInfo cmd;
// 				cmd.iCommandType = CMD_RESETCMDINDEX;
// 				SMC.setCmdInfo(cmd);
// 			    memset(&g_pci, 0, sizeof(g_pci));
// 			    index = 0;
// 			}
// 
// 		}
// 		g_pci.nCmdCount++;
//  		g_pci.dwCmdList[index] = MAKELONG(lineno-1, ci);
// 
// 		MWM.set_prog_command_info(g_pci);
// 	}
// }

int ADD_PROG_COMMAND(int lineno, int ci, bool isdelay)		
{
	int nRet = 0;
	if ( SMC.isCodeSysMotion ) // lock lua script loop
	{
		while( SMC.is_queue_full() )
		{
			if(	do_events() != 0 )
			{
				return nRet;
			}
			Sleep(1);
		}
		return nRet;
	}
/**
	if( ci >= 0 )
	{
		ProgCommandInfo g_pci = MWM.get_prog_command_info();

		unsigned int index = g_pci.nCmdCount;
// 		if ( index > MAX_PROG_COMMAND-1)
// 		{
// 			index = 0;
// 		}

    	g_pci.dwCmdList[index] = MAKELONG(lineno-1, ci);
		g_pci.nCmdCount++;
		MWM.set_prog_command_info(g_pci);

		if ( isdelay )
		{
			last_command_index = 0; // disable trigger when trigger behind waittime command
		}
		else
			last_command_index = ci; // for trigger

		// avoid runtime-51 error, buffer overwrite.
		if ( index == MAX_PROG_COMMAND-1 )
		{
			if ( wait_motion_finish() ) // lock thread, wait all motion finished
			{
				CmdInfo cmd;
				cmd.iCommandType = CMD_RESETCMDINDEX;
				SMC.setCmdInfo(cmd);
				memset(&g_pci, 0, sizeof(g_pci));
				MWM.set_prog_command_info(g_pci);
				SMC.clear_trigger();
			}
		}
	}
*/

	static unsigned long cnttest = 0;

	if( ci >= 0 )
	{
		ProgCommandInfo g_pci = MWM.get_prog_command_info();

		unsigned int index = g_pci.nCmdCount;
		// avoid runtime-51 error, buffer overwrite.
		if ( index > MAX_PROG_COMMAND-1 )
		{
			printf("\n\n******* PMC: reset pointer command line to 0 [ times = %d ] **********\n\n", ++cnttest );
			nRet = 1;

			index = 0;
			memset(&g_pci, 0, sizeof(g_pci));
			if ( wait_motion_finish() ) // lock thread, wait all motion finished
			{
				CmdInfo cmd;
				cmd.iCommandType = CMD_RESETCMDINDEX;
				SMC.setCmdInfo(cmd);
 				SMC.clear_trigger();
			}

		}

		g_pci.dwCmdList[index] = MAKELONG(lineno-1, ci);
		g_pci.nCmdCount++;
		MWM.set_prog_command_info(g_pci);

		if ( isdelay )
		{
			last_command_index = 0; // disable trigger when trigger behind waittime command
		}
		else
			last_command_index = ci; // for trigger
 		 
	}

	return nRet;
}

// Implementation of script functions
bool wait_motion_finish()
{
	if ( is_command_ignored() )
 		return true;
 
	cout << "wait motion stop---------" << endl;
	int cnt = 0;
	if ( MWM.is_main_world_model_available() )
	{
		MWM.set_wait_signal(true);
		if ( SMC.isCodeSysMotion )
		{
			cmd.iCommandType = CMD_WAITMOTIONFINISH;
			SMC.setCmdInfo(cmd);
		}		
 
		unsigned int cnt = 0;
 		do // stop 状态更新有延迟
 		{
			cmd.iCommandType = CMD_TIMEDELAY;
			cmd.iDelayTime   = 1;
			SMC.setCmdInfo(cmd);

			if ( SMC.isCodeSysMotion )
			{
				Sleep(20);
			}			

			if ( ProgramEditor::is_running_flag == false )
			{
 				cout << "wait end 22 ---------" << endl;
				break;
			}
			if ( do_events() != 0 )
			{
				break;
			}			
			
			if ( MWM.get_robot_state().iGroupStatus == GrpHold )
			{
// 				cmd.iCommandType = CMD_TIMEDELAY;
// 				cmd.iDelayTime   = 1;
// 				SMC.setCmdInfo(cmd);
				continue;
			}
// 			cout << " wait count = " << cnt++ << '\n';

			MotionCoreInfo mc = MWM.get_robot_state(true).mc_info;
			if ( mc.iMotionStatus == MotionCoreInfo::msSTOP )
			{
				if ( SMC.isCodeSysMotion )
				{
					if ( cnt++ > 10 )
					{
						break;
					}
				}
				else
 					break;
			}
  
// 			MCS_TimeDelay(2);
// 			CString msg;
// 			msg.Format(_T("motion status = %d\n"), static_cast<int>( MWM.get_group_info().iMotionStatus ) );
// 			TRACE(msg);
		}while(true);
	}
	cout << "wait end---------" << endl;
	MWM.set_wait_signal(false);
	if ( ProgramEditor::is_running_flag )
	{
		program_dialog->editor_.HighlightLine(edit_line_no-1);
	}

 	return true;
}

int WaitTime(long lMilliseconds)
{
	if ( is_command_ignored() )
		return true;

	static long counttext = 0;

	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	printf("\n&&&&& do wait time = %d; currTime = %d-%02d-%02d %02d:%02d:%02d &&&&&\n", lMilliseconds, 
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond);
  	//std::cout << "do wait time = " << lMilliseconds << endl;

	if ( SMC.isCodeSysMotion )
	{
		bool use_codesys_wait = true;
		if ( use_codesys_wait )
		{
			int nRet = WaitTimeCodeSys(lMilliseconds);
			return nRet;
		}
	}

	cmd.iObjNo = 0;
	cmd.iSourceLineNo = edit_line_no;
	cmd.iCommandType = CMD_DELAYING;
	cmd.iDelayTime   = lMilliseconds;
	int  nCmdIndex = SMC.setCmdInfo(cmd); // set state flag.
	int nRet = ADD_PROG_COMMAND(edit_line_no, nCmdIndex, true);
    Sleep(10); // 延迟检测状态
	unsigned int cnt = 0;
	// run condition.
	bool inStateCheck = false;
	bool inTimeCount = false;
	do  
	{
		if ( !inStateCheck )
		{
			inStateCheck = true;
			printf("\n------in state check for wait time------\n");
 		}

		Sleep(1);
		if ( ProgramEditor::is_running_flag == false )
		{
			break;
		}
		if ( do_events() != 0 )
		{
			break;
		}			
 
		MotionCoreInfo mc = MWM.get_robot_state(false).mc_info;
		if ( mc.iMotionStatus == MotionCoreInfo::msDELAYING )
		{
			printf("\n------state delaying break cycle------\n");
   			break;
		}
		if ( mc.iMotionStatus == MotionCoreInfo::msSTOP ) //nRet == 1 &&
		{
			printf("\n------state stop break cycle------\n");
			break;
		}
  	}while(true);

	// timer to lock script cursor;
	Ysbot::Time elpseTime;
	long maxTime = lMilliseconds;
	elpseTime.update();
	bool bsettime = false;

	do  
	{
		if ( !inTimeCount )
		{
			inTimeCount = true;
			printf("\n********in time count for wait time.********\n");
		}
		Sleep(1);
		if ( ProgramEditor::is_running_flag == false )
		{
			break;
		}
		if ( do_events() != 0 )
		{
			break;
		}			

		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
			elpseTime.update();
			continue;
		}
 
		if ( elpseTime.elapsed_msec() > maxTime )
		{
			break;
		}
 
	}while(true);
	MWM.update_robot_state(cmd); // reset state flag.

	printf("\n####### wait time = %d finished!#######\n",++counttext);

   	return nCmdIndex;
}

int WaitTimeCodeSys(long lMilliseconds)
{
	if ( is_command_ignored() )
		return true;

	std::cout << "do wait time = " << lMilliseconds << endl;
	cmd.iSourceLineNo = edit_line_no;
	cmd.iCommandType = CMD_DELAYING;
	cmd.iDelayTime   = lMilliseconds;
	cmd.iObjNo = 1;
	int  nCmdIndex = SMC.setCmdInfo(cmd); // set state flag.
	ADD_PROG_COMMAND(edit_line_no, nCmdIndex, true);
	return nCmdIndex;
}

void SPEED(double dfSpeed)
{
	// range: 0.01 ~ 100
	if ( is_command_ignored() )
		return;

}

void BLEND(int nOnOff)
{
	if ( is_command_ignored() )
		return;
}

// void MOVEP(PosInfo ap)
// {
// 	cout << " do ptp " << ap.axis.q1 << endl;
// 	CmdInfo cmd;
// 	cmd.iCommandType = CMD_PTP;
// 	cmd.destAxisPos = ap;
// 	SMC.setCmdInfo(cmd);
// }

void MOVEP(PosInfo ap)
{
    MOVEP(ap,NULL,NULL);
}

void MOVEP(PosInfo ap, Dynamic* dyn)
{
	MOVEP(ap,dyn,NULL);
}

void MOVEP(PosInfo ap, Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

 	if ( ProgramEditor::is_running_flag == false )
 	{
		LSCRIPT->free_lua();
 		return;
 	}
	cout << " do point to point " << ap.axis.q1 << endl;
	cmd.iCommandType = CMD_PTP;
	CartRef cref = MWM.get_prog_cart().refSys;
	CartRef tool = MWM.get_prog_tool().refSys;
	cmd.cartref = &cref;
	cmd.tool    = &tool.cpos;

	if ( strcmp( cref.cartname, "PCS_2" ) == 0 )
	{
		ap.pos_type = PosInfo::CTproduct_2;
	}
	else if ( strcmp( cref.cartname, "PCS_1" ) == 0 )
	{
		ap.pos_type = PosInfo::CTproduct_1;
	}

	cmd.destPos = ap;
	if (dyn)
 		cmd.dyn = *dyn;
	else
	{
		cmd.dyn = MWM.get_dynamic_program();
 	}

 	if (ovl)
 		cmd.ovl = *ovl;
	else
	{
		cmd.ovl = MWM.get_overlap_program();
	}

	if ( SMC.isCodeSysMotion == false && ap.pos_type == PosInfo::CTmachine )
	{
		KINE.inv_kine(ap.cart,&ap.axis);
	}	
	start_pos = end_pos;
	end_pos   = ap;
	cmd.iSourceLineNo = edit_line_no;

 	int nCmdIndex = SMC.setCmdInfo(cmd);
	ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
// 	std::cout << SMC.get_error();
}

void MOVEPREL(PosInfo ap)
{
	MOVEPREL(ap,NULL,NULL);
}

void MOVEPREL(PosInfo ap, Dynamic* dyn)
{
	MOVEPREL(ap,dyn,NULL);
}

void MOVEPREL(PosInfo ap, Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

	if ( ProgramEditor::is_running_flag == false )
	{
		LSCRIPT->free_lua();
		return;
	}
	cout << " do point to point " << ap.axis.q1 << endl;
	cmd.iCommandType = CMD_PTPREL;
	CartRef cref = MWM.get_prog_cart().refSys;
	CartRef tool = MWM.get_prog_tool().refSys;
	cmd.cartref = &cref;
	cmd.tool    = &tool.cpos;

	cmd.destPos = ap;
	if (dyn)
		cmd.dyn = *dyn;
	else
	{
		cmd.dyn = MWM.get_dynamic_program();
	}

	if (ovl)
		cmd.ovl = *ovl;
	else
	{
		cmd.ovl = MWM.get_overlap_program();
	}

	if ( SMC.isCodeSysMotion == false && ap.pos_type == PosInfo::CTmachine )
	{
		KINE.inv_kine(ap.cart,&ap.axis);
	}	
	start_pos = end_pos;
	end_pos   = ap;
	cmd.iSourceLineNo = edit_line_no;
	int nCmdIndex = SMC.setCmdInfo(cmd);
	ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
	// 	std::cout << SMC.get_error();
}

void MOVEL(PosInfo cp)
{
	MOVEL(cp,NULL,NULL);
}

void MOVEL(PosInfo cp, Dynamic* dyn)
{
	MOVEL(cp,dyn,NULL);
}

void MOVEL( PosInfo ap, Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

 	cout << " do line " << endl;

	cmd.iCommandType = CMD_LIN;
	CartRef cref = MWM.get_prog_cart().refSys;
	CartRef tool = MWM.get_prog_tool().refSys;
	cmd.cartref = &cref;
	cmd.tool    = &tool.cpos;

	if ( strcmp( cref.cartname, "PCS_2" ) == 0 )
	{
		ap.pos_type = PosInfo::CTproduct_2;
	}
	else if ( strcmp( cref.cartname, "PCS_1" ) == 0 )
	{
		ap.pos_type = PosInfo::CTproduct_1;
	}

	cmd.destPos = ap;
	if (dyn)
		cmd.dyn = *dyn;
	else
	{
		cmd.dyn = MWM.get_dynamic_program();
	}

	if (ovl)
		cmd.ovl = *ovl;
	else
	{
		cmd.ovl = MWM.get_overlap_program();
	}

	if ( SMC.isCodeSysMotion == false && ap.pos_type == PosInfo::CTmachine )
	{
		KINE.inv_kine(ap.cart,&ap.axis);
	}	
	start_pos = end_pos;
	end_pos   = ap;
	cmd.iSourceLineNo = edit_line_no;

	int nCmdIndex = SMC.setCmdInfo(cmd);
	ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
}

void MOVELREL(PosInfo cp)
{
	MOVELREL(cp,NULL,NULL);
}

void MOVELREL(PosInfo cp, Dynamic* dyn)
{
	MOVELREL(cp,dyn,NULL);
}

void MOVELREL( PosInfo ap, Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

	cout << " do line " << endl;

	cmd.iCommandType = CMD_LINREL;
	CartRef cref = MWM.get_prog_cart().refSys;
	CartRef tool = MWM.get_prog_tool().refSys;
	cmd.cartref = &cref;
	cmd.tool    = &tool.cpos;

	cmd.destPos = ap;
	if (dyn)
		cmd.dyn = *dyn;
	else
	{
		cmd.dyn = MWM.get_dynamic_program();
	}

	if (ovl)
		cmd.ovl = *ovl;
	else
	{
		cmd.ovl = MWM.get_overlap_program();
	}

	if ( SMC.isCodeSysMotion == false && ap.pos_type == PosInfo::CTmachine )
	{
		KINE.inv_kine(ap.cart,&ap.axis);
	}	
	start_pos = end_pos;
	end_pos   = ap;
	cmd.iSourceLineNo = edit_line_no;

	int nCmdIndex = SMC.setCmdInfo(cmd);
	ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
}

void MOVEC( PosInfo auxpi, PosInfo destpi )
{
	MOVEC(auxpi,destpi,NULL,NULL);
}

void MOVEC( PosInfo auxpi, PosInfo destpi, Dynamic* dyn  )
{
    MOVEC(auxpi,destpi,dyn,NULL);
}

void MOVEC( PosInfo auxpi, PosInfo destpi, Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

	cout << " do circle motion " << endl;

	cmd.iCommandType = CMD_CIR;
	CartRef cref = MWM.get_prog_cart().refSys;
	CartRef tool = MWM.get_prog_tool().refSys;
	cmd.cartref = &cref;
	cmd.tool    = &tool.cpos;

	cmd.auxPos  = auxpi;
	cmd.destPos = destpi;
	if (dyn)
		cmd.dyn = *dyn;
	else
	{
		cmd.dyn = MWM.get_dynamic_program();
	}

	if (ovl)
		cmd.ovl = *ovl;
	else
	{
		cmd.ovl = MWM.get_overlap_program();
	}
	cmd.iSourceLineNo = edit_line_no;
	int nCmdIndex = SMC.setCmdInfo(cmd);

	ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
}

void MOVECREL( PosInfo auxpi, PosInfo destpi )
{
	MOVECREL(auxpi,destpi,NULL,NULL);
}

void MOVECREL( PosInfo auxpi, PosInfo destpi, Dynamic* dyn  )
{
	MOVECREL(auxpi,destpi,dyn,NULL);
}

void MOVECREL( PosInfo auxpi, PosInfo destpi, Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

	cout << " do circle motion " << endl;

	cmd.iCommandType = CMD_CIRREL;
	CartRef cref = MWM.get_prog_cart().refSys;
	CartRef tool = MWM.get_prog_tool().refSys;
	cmd.cartref = &cref;
	cmd.tool    = &tool.cpos;

	cmd.auxPos  = auxpi;
	cmd.destPos = destpi;
	if (dyn)
		cmd.dyn = *dyn;
	else
	{
		cmd.dyn = MWM.get_dynamic_program();
	}

	if (ovl)
		cmd.ovl = *ovl;
	else
	{
		cmd.ovl = MWM.get_overlap_program();
	}
	cmd.iSourceLineNo = edit_line_no;
	int nCmdIndex = SMC.setCmdInfo(cmd);

	ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
}

//??????????????????????
void GetRobPos(PosInfo* pi)
{
	if ( is_command_ignored() )
		return;

	if ( pi->pos_type == PosInfo::CTaxis )
	{
		pi->set_axis(AxisPos(1,2,3,4,5,6),AxisPos(1,2,3,4,5,6));
 	}
	else if ( pi->pos_type == PosInfo::CTmachine )
	{
		pi->set_cart( CartPos(1,2,3,4,5,6,5), AxisPos(1,2,3,4,5,6) );
 	}
	 

}

void creat_func_table()
{
    ;
}

void OUTIO( int deviceType, int nb, bool val, bool inqueue/* = false*/, double dfDelayTime/* = 0 */ )
{
	if ( is_command_ignored() )
		return;

	do_out( nb, val, deviceType, inqueue, dfDelayTime );
}

int do_out( int nb, bool val, int deviceType, 
	        bool inqueue       /* = false */, 
			double dfDelayTime /* = 0 */)
{
	if ( is_command_ignored() )
		return 0;

	IO_DEVICE ice = IO_DEVICE(deviceType);
	int nCmdIndex = 0;
	cmd.iSourceLineNo = edit_line_no;
// 	if ( inqueue )
// 	{
// 		if ( ice == MCS_GPIO || ice == MCS_REMOTEIO )
// 		{
//  
// 			nCmdIndex = SMC.mc_dout( nb, val ? 1: 0, ice, true, dfDelayTime );
// 
// 			if ( SMC.isCodeSysMotion == false )
// 			{
// 				ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
// 			}
// 
// // 			printf(" dout set line = %d, nCmdIndex = %d\n", edit_line_no, nCmdIndex );
//  			return nCmdIndex;
//  		}
// 		else  
// 		{
//  			if ( inqueue && wait_motion_finish() == false )
// 				return -1;
// 
// 			if ( ProgramEditor::is_running_flag )
// 			{
// 				program_dialog->editor_.HighlightLine(edit_line_no-1);
// 			}
// 		}
//  	}
//  
// 	if ( ice == MCS_GPIO || ice == MCS_REMOTEIO || ice == MCS_LOCALIO )
// 	{
// 		return SMC.mc_dout(nb,val,ice,false,dfDelayTime);
// 	}
// 	else if ( ice == PLC_DEVICE )
// 	{
// 		return FieldBusDevice::get_main_fieldbus_device().dout(nb,val);
// 	}
	 
	if ( inqueue )
	{
		if ( ice == PLC_DEVICE || ice == MCS_LOCALIO || ( SMC.isCodeSysMotion && ice == MCS_REMOTEIO ) )
		{
			if ( wait_motion_finish() == false )
				return -1;

			if ( ProgramEditor::is_running_flag )
			{
				program_dialog->editor_.HighlightLine(edit_line_no-1);
			}
		}
	}

	if ( ice == PLC_DEVICE )
	{
		return FieldBusDevice::get_main_fieldbus_device().dout(nb,val);
 	}
	else
	{
		nCmdIndex = SMC.mc_dout( nb, val, ice, inqueue, dfDelayTime );
		if ( SMC.isCodeSysMotion == false )
		{
			ADD_PROG_COMMAND(edit_line_no, nCmdIndex);
		}
  		return nCmdIndex;
 	}

	return nCmdIndex;
}

bool INIO( int deviceType, unsigned int nb, bool inqueue /* = false */,  bool bVirtual/* = false */)
{
	if ( is_command_ignored() )
		return false;

    return do_in( nb, deviceType, inqueue, bVirtual );
}

bool do_in( int nb, int deviceType, bool inqueue /* = false */,  bool bVirtual/* = false */ )
{
	if ( is_command_ignored() )
		return false;

	IO_DEVICE ice = IO_DEVICE(deviceType);

	bool bval = false;
	int ret = 0;
	cmd.iSourceLineNo = edit_line_no;
	if ( inqueue )
	{
		if ( SMC.isCodeSysMotion )
		{
			cmd.iCommandType = CMD_WAITMOTIONFINISH;
			SMC.setCmdInfo(cmd);
		}

		if ( wait_motion_finish() == false )
			return false;
 
		if ( ProgramEditor::is_running_flag )
		{
			program_dialog->editor_.HighlightLine(edit_line_no-1);
		}
	}

	if ( bVirtual )
	{
// 		if ( Inout_Dialog )
// 		{
//    			ret = Inout_Dialog->get_virtual_input( deviceType, nb, &bval );
//  		}
		if ( m_pwndDlgIO )
		{
			ret = m_pwndDlgIO->get_virtual_input( deviceType, nb, &bval );
		}
	}
	else
	{
		if ( ice == PLC_DEVICE )
		{
			FieldBusDevice::get_main_fieldbus_device().din(nb,&bval);
		}
		else
		{
			unsigned short ival = 0;
			ret = SMC.mc_din( nb, &ival,ice );
			if ( ret >= 0 )
				bval = (ival==1)? true : false;
		}
 	}
 
	return bval;
  
}

int WAITIO( int deviceType, unsigned int nb, unsigned int val, long et, bool bVirtual/* = false */  )
{
	if ( is_command_ignored() )
		return 0;

	IO_DEVICE ice = IO_DEVICE(deviceType);
 
	int nRet = 0;
	unsigned short valTemp = 0;
//    	const InOutObjectList& iof = MWM.get_io_objects(ice);
// 	unsigned int num = MWM.get_io_module_parameters(ice).x_point_num;
// 	if ( nb >= num )
//  		return -1;
	cmd.iSourceLineNo = edit_line_no;
// 

	if ( wait_motion_finish() )
	{
		cmd.iCommandType = CMD_WAITIO;
		cmd.iInOutNo     = nb;
		cmd.iIoValue     = val;
		MWM.update_robot_state(cmd);

		if ( SMC.isCodeSysMotion )
		{
 			SMC.setCmdInfo(cmd);
		}

		if ( ProgramEditor::is_running_flag )
		{
			program_dialog->editor_.HighlightLine(edit_line_no-1);
		}
	}
	else
		return -1;

	cout << "wait io start---------" << endl;
	MWM.set_wait_signal(true);

 	bool bval = false;
	Ysbot::Time elpstime;
	double sectimeout = et;
	double difftime   = 0; 
	do 
	{
		Sleep(1);

		if ( ProgramEditor::is_running_flag == false )
		{
			MWM.set_wait_signal(false);
// 			MWM.update_robot_state(cmd);
			return -1;
		}
		if(	do_events() != 0 )
		{
			MWM.set_wait_signal(false);
// 			MWM.update_robot_state(cmd);
			return -1;
		}

		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
			sectimeout = difftime;
			elpstime.update();
  			continue;
		}

		if ( bVirtual )
		{
// 			nRet = Inout_Dialog->get_virtual_input( ice, nb, &bval );
			if ( m_pwndDlgIO )
			{
				nRet = m_pwndDlgIO->get_virtual_input(ice,nb,&bval);
			}
			valTemp = static_cast<unsigned short>(bval);
 		}
		else
		{
 			if ( ice == PLC_DEVICE )
			{
				FieldBusDevice::get_main_fieldbus_device().din(nb,&bval);
				valTemp = static_cast<unsigned short>(bval);
			}
			else
			{
				SMC.mc_din( nb, &valTemp, ice );
			}
		}
  
		nRet = static_cast<int>(valTemp); // ????????????

		if ( sectimeout > 0 )
		{
			difftime = sectimeout - elpstime.elapsed_msec();
			if ( difftime < 0 )
			{
 				MSD.ShowInfo(_T("等待IO输入超时！"),JTWARNING);
 				nRet = -1; // 
 				break;
			}
			else
			{
				if ( ProgramEditor::is_running_flag )
				{
					program_dialog->editor_.HighlightLine(edit_line_no-1);
				}
			    cout << " wait msec = " << elpstime.elapsed_msec() << '\n';
			}
 		} 
 
		if ( MWM.get_robot_state().iGroupStatus != GrpWait )
		{
			cmd.iCommandType = CMD_WAITIO;
			cmd.iInOutNo     = nb;
			cmd.iIoValue     = val;
			MWM.update_robot_state(cmd);
		}
	
	} while ( valTemp != val );

	cout << "wait io end---------" << endl;
	MWM.set_wait_signal(false);

 	MWM.update_robot_state(cmd);

	return nRet;
}

bool WaitIsFinished()
{
	if ( is_command_ignored() )
		return true;

	if ( ProgramEditor::is_running_flag ) // && MWM.get_wait_signal() == false
	{
		wait_motion_finish();
	}
//   	DELAY(300);
	return true;
}

void Info( const char* msg )
{
	if ( is_command_ignored() )
		return;

	MSD.ShowInfo (CString(msg), JTMESSAGE );
 
	if ( ProgramEditor::is_running_flag )
	{
		// 		program_dialog->editor_.HighlightLine(edit_line_no-1);
	}
}

// option time 
int on_distance_dout( int disttype, double dist, DigitalOutput* dout, bool outval )
{
	if ( is_command_ignored() )
		return 0;

	if (last_command_index == 0 )
 		return -1;
 
	Trigger trigger;
	trigger.trigger_type = onDISTANCE;
	trigger.distance_type = ( disttype == 0 ? FROMBEGIN : FROMEND );
	trigger.distance = dist;
	trigger.dout     = *dout;
	trigger.dout.output_value = outval;
	trigger.command_index = last_command_index;

	KINE.fwd_kine( start_pos.axis, &start_pos.cart );
	KINE.fwd_kine( end_pos.axis, &end_pos.cart );

	trigger.path_start = start_pos;
	trigger.path_end   = end_pos;

	SMC.add_trigger(trigger);

	return 0;
}

int on_position_dout( double percent, DigitalOutput* dout, bool outval )
{
	if ( is_command_ignored() )
		return 0;

	if (last_command_index == 0 )
		return -1;

	Trigger trigger;
	trigger.trigger_type = onPOSITION;
	trigger.path_percent = percent;
	trigger.dout     = *dout;
	trigger.dout.output_value = outval;
	trigger.command_index = last_command_index;

	KINE.fwd_kine( start_pos.axis, &start_pos.cart );
	KINE.fwd_kine( end_pos.axis, &end_pos.cart );

	trigger.path_start = start_pos;
	trigger.path_end   = end_pos;

	SMC.add_trigger(trigger);
	return 0;
}

bool on_position_din( double percent, int nb, int deviceType )
{
	if ( is_command_ignored() )
		return false;

	if (last_command_index == 0 )
		return false;

 	IO_DEVICE ice = IO_DEVICE(deviceType);

	CartPos cpos = (end_pos.cart) - (start_pos.cart);
	double path_length = cpos.pos.length();
	PosInfo rpos;
	std::cout << " go into read trigger " << '\n';

	do 
	{
		Sleep(1);

		if ( ProgramEditor::is_running_flag == false )
		{
			MWM.set_wait_signal(false);
			return false;
		}

		if(	do_events() != 0 )
		{
			MWM.set_wait_signal(false);
			return false;
		}

		const MotionCoreInfo& mcinfo = MWM.get_robot_state(true).mc_info; //  状态更新
 		 
 		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
  			continue;
 
		if ( last_command_index == mcinfo.nCommandIndex )
		{
			rpos = SMC.get_position( PosInfo::CTmachine, LDScommad );
			CartPos posvec = rpos.cart - (start_pos.cart);
			double curr_dist = posvec.pos.length();

			std::cout << " trigger percent = " << curr_dist/path_length * 100 << '\n';
			if ( path_length == 0 )
				break;
			else
			{
				if ( (curr_dist/path_length) >= (percent*0.01) )
				{
					break;
				}
			}
		}
 	
	} while (true);

	std::cout << " go out read trigger " << '\n';

	if ( ProgramEditor::is_running_flag )
	{
		program_dialog->editor_.HighlightLine(edit_line_no-1);
	}

	bool bval = false;

 	bval = do_in( nb, ice, false );
	 
	std::cout << " trigger percent : read = " << bval << '\n';
 	return bval;
}

// 在下一个运动段的某点触发
int on_parameter_dout( double timecount, DigitalOutput* dout, bool outval )
{
	if ( is_command_ignored() )
		return 0;
	return 0;
}

void SET_DYNAMIC(Dynamic* dyn)
{
	if ( is_command_ignored() )
		return;
	cout << "set dynamic \n";
	MWM.set_dynamic_program(*dyn);
}

void SET_OVERLAP(Overlap* ovl)
{
	if ( is_command_ignored() )
		return;

	cout << "set Overlap \n";
	MWM.set_overlap_program(*ovl);
}

void SET_REFCART(CartRef* cref)
{
	if ( is_command_ignored() )
		return;

	cout << "set CartRef \n";
	RefSysSetlInfo refinfo;
	refinfo.refSet = SVAR_PROGRAM;
	std::string varname = cref->getSelfName();
	if ( varname.find(STR_LABEL_SYSTEM) != std::string::npos )
	{
		refinfo.refSet = SVAR_SYSTEM;
	}
	else if ( varname.find(STR_LABEL_GLOBAL) != std::string::npos )
	{
		refinfo.refSet = SVAR_GLOBAL;
	}
	 
	refinfo.refSys = *cref;
	MWM.set_prog_cart(refinfo);
}

void SET_TOOL(CartRef* cpos)
{
// 	if ( cpos == NULL )
//  		return;
	if ( is_command_ignored() )
		return;

 	cout << "set TOOL \n";
 
	RefSysSetlInfo refinfo;
	refinfo.refSet = SVAR_PROGRAM;
	std::string varname = cpos->getSelfName();
	if ( varname.find(STR_LABEL_SYSTEM) != std::string::npos )
	{
		refinfo.refSet = SVAR_SYSTEM;
	}
	else if ( varname.find(STR_LABEL_GLOBAL) != std::string::npos )
	{
		refinfo.refSet = SVAR_GLOBAL;
	}
	refinfo.refSys = *cpos;
	MWM.set_prog_tool(refinfo);
}

void TRACKVIS( Dynamic* dyn )
{
	TRACKVIS( dyn, NULL );
}

void TRACKVIS( Dynamic* dyn, Overlap* ovl )
{
	if ( is_command_ignored() )
		return;

	do 
	{
		Sleep(1);

		if ( ProgramEditor::is_running_flag == false )
		{
			MWM.set_wait_signal(false);
			return;
		}

		if(	do_events() != 0 )
		{
			MWM.set_wait_signal(false);
			return;
		}

		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
 			continue;
		}

		const VisionPos& vispos = MWM.get_vision_pos();
		if ( vispos.pos_known )
		{
			PosInfo vispi;
			vispi.valid = true;
			vispi.pos_type = PosInfo::CTmachine;
			vispi.cart = vispos.cpos;
			MOVEP(vispi,dyn,ovl);
			cout << "move to vision location = " << vispi.cart << '\n';
			break;
		}
		else
		{
			cout << "wait vision found object !\n";
		}
 	} while ( true );
}

int WaitObjectWithVisionSensor( SensableObject* obj )
{
	if ( is_command_ignored() )
		return 0;

	if ( wait_motion_finish() )
	{
		if ( ProgramEditor::is_running_flag )
		{
			program_dialog->editor_.HighlightLine(edit_line_no-1);
		}
	}
	else
		return -1;

	cmd.iCommandType = CMD_WAITIO;
	MWM.update_robot_state(cmd);
	MWM.set_wait_signal(true);

	do 
	{
		Sleep(1);

		if ( ProgramEditor::is_running_flag == false )
		{
			MWM.set_wait_signal(false);
			return -1;
		}

		if(	do_events() != 0 )
		{
			MWM.set_wait_signal(false);
			return -1;
		}

		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
			continue;
		}

		const VisionPos& vispos = MWM.get_vision_pos();
		if ( vispos.pos_known || obj->simulate_obj )
		{
//  			posinfo->valid = true;
// 			posinfo->pos_type = PosInfo::CTmachine;
// 			posinfo->cart = vispos.absPosRefWorkCoord; 
			if ( obj->simulate_obj == false )
			{
				obj->set_pos(vispos.absPosRefWorkCoord);// reference work coordinate, ie PCS
 			}
			MWM.update_robot_state(cmd);
 			break;
		}
		else
		{
			cout << "wait vision found object !\n";
		}

	} while ( true );

	MWM.set_wait_signal(false);

	return 0;
}

void LOADSUB( const char* modname )
{
	if ( is_command_ignored() )
		return;

	char chHome[512] = {'\0'};
	LVAR->load_subprogram_var( modname, chHome ); // 导入子模块的变量，返回工作文件夹的路径
	string strpath = string(chHome) + "?.prg";
	 
	char chCmd[512];
	sprintf( chCmd,"package.path=package.path .. \";%s\"; \r\n", strpath.c_str() ); // 添加外部模块的路径到环境中
	string strCmd = string(chCmd);
 	sprintf( chCmd, "require (\"%s\"); \r\n ", modname ); // 加载模块
	strCmd += string(chCmd);
	LSCRIPT->doString(strCmd);

}


int ConveyorWaitObject(int iCoordType)
{
	if ( is_command_ignored() )
		return 0;
	iConveyorCoordType = iCoordType;
	cmd.iSourceLineNo = edit_line_no;
	// 

	if ( wait_motion_finish() )
	{
		if ( ProgramEditor::is_running_flag )
		{
			program_dialog->editor_.HighlightLine(edit_line_no-1);
		}
	}
	else
		return -1;

	cmd.iCommandType = CMD_WAITIO;
	MWM.update_robot_state(cmd);


	cout << "TRACK: wait object start---------" << endl;
	MWM.set_wait_signal(true);

	bool bval = false;
	Ysbot::Time elpstime;
	double difftime   = 0; 
	 
 	do 
	{
		Sleep(1);

		if ( ProgramEditor::is_running_flag == false )
		{
			MWM.set_wait_signal(false);
  			lastConveyorWaitFlag[iCoordType] = 0 ;
			return -1;
		}
		if(	do_events() != 0 )
		{
			MWM.set_wait_signal(false);
 			lastConveyorWaitFlag[iCoordType] = 0;
 
			return -1;
		}

		const MotionCoreInfo& mc = MWM.get_motion_core_info(); // go into work area, robot can catch up.
		if ( mc.iMemData[iCoordType] == 1 && lastConveyorWaitFlag[iCoordType] == 0 )
		{
			lastConveyorWaitFlag[iCoordType] = 1;
			MWM.update_robot_state(cmd);
			break;
		}

	} while ( true );

	cout << "TRACK: wait object end---------" << endl;
	MWM.set_wait_signal(false);


	return 0;
}

void ConveyorDone()
{
	lastConveyorWaitFlag[iConveyorCoordType] = 0;
}

// static FUNCADDRESS g_ScriptFunc[] =
// {
// 	"DELAY",    &DELAY, 
// 	"SPEED",	&SPEED,
// 	"BLEND",	&BLEND,
// 	"PTP",		&MOVEP,
// 	"Lin",		&MOVEL,
// };
  
 

// long GetScriptFuncTable( int& cmd_count )	 
// {
// 	cmd_count = ARRAY_SIZE(g_ScriptFunc);
// 	return (long)(g_ScriptFunc);	 
// }
}