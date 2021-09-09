// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _SCRIPTFUNC_H_
#define _SCRIPTFUNC_H_

#include "../CmdInfo.h"
#include "LuaState.h"
#include "../InOutObject.h"
#include "../DigitalOutput.h"
#include "../SensableObject.h"

// note: use variable from outside text, parameter must use pointer.
namespace ScriptFunc
{
 	int luaHook(  const char*, const char* short_src, int lineid,lua_State *L ); // lua call function

	void init_script_function();

	int ADD_PROG_COMMAND(int lineno, int ci, bool isdelay = false);	// user custom function.	
	 
	bool wait_motion_finish();

	int  WaitTime(long lMilliseconds);
	int  WaitTimeCodeSys( long lMilliseconds);

	void SPEED(double dfSpeed);
	void BLEND(int nOnOff);
 
	void MOVEP ( PosInfo );
	void MOVEP ( PosInfo, Dynamic* );
	void MOVEP ( PosInfo, Dynamic* = NULL, Overlap* = NULL );

	void MOVEPREL ( PosInfo );
	void MOVEPREL ( PosInfo, Dynamic* );
	void MOVEPREL ( PosInfo, Dynamic* = NULL, Overlap* = NULL );

	void MOVEL ( PosInfo );
	void MOVEL ( PosInfo, Dynamic* );
	void MOVEL ( PosInfo, Dynamic* = NULL, Overlap* = NULL );

	void MOVELREL ( PosInfo );
	void MOVELREL ( PosInfo, Dynamic* );
	void MOVELREL ( PosInfo, Dynamic* = NULL, Overlap* = NULL );

	void MOVEC ( PosInfo auxpi, PosInfo destpi );
	void MOVEC ( PosInfo auxpi, PosInfo destpi, Dynamic* );
	void MOVEC ( PosInfo auxpi, PosInfo destpi, Dynamic* = NULL, Overlap* = NULL );

	void MOVECREL ( PosInfo auxpi, PosInfo destpi );
	void MOVECREL ( PosInfo auxpi, PosInfo destpi, Dynamic* );
	void MOVECREL ( PosInfo auxpi, PosInfo destpi, Dynamic* = NULL, Overlap* = NULL );

	void SET_DYNAMIC (Dynamic*);
	void SET_OVERLAP (Overlap*);
	void SET_REFCART (CartRef*);
	void SET_TOOL    (CartRef*);

	void OUTIO(int deviceType, int nb, bool val, bool inqueue = false, double dfDelayTime = 0 );
	int  do_out(int nb, bool val, int deviceType = MCS_GPIO, bool inqueue = false, double dfDelayTime = 0 );
	bool INIO( int deviceType, unsigned int nb,  bool inqueue = false/* = false */,  bool bVirtual = false/* = false */); // need transfer pointer
	bool do_in(int nb, int deviceType = MCS_GPIO, bool inqueue = false, bool bVirtual = false );
 
	int  WAITIO ( int deviceType, unsigned int nb, unsigned int val, long et, bool bVirtual = false );

	bool WaitIsFinished();

	void GetRobPos(PosInfo*); // 
	void Info(const char* msg );

	void creat_func_table();

	// trigger
	int on_distance_dout(  int disttype, double dist, DigitalOutput* dout, bool outval );
	int on_position_dout( double percent, DigitalOutput* dout, bool outval );
	int on_parameter_dout( double timecount, DigitalOutput* dout, bool outval );
// 	bool on_position_din( double percent, DigitalInput* din );
	bool on_position_din( double percent, int nb, int deviceType = MCS_GPIO );

	void TRACKVIS(Dynamic* = NULL);
	void TRACKVIS(Dynamic* = NULL, Overlap* = NULL);
	void LOADSUB( const char* );

	static char const* ScriptFuncTable[] =
	{
		"DELAY",  
		"SPEED", 
		"BLEND", 
		"PTP",	 
		"Lin",	 
	};
    
	extern long edit_line_no;

	BOOL PeekAndPump();

	bool is_command_ignored ();

	int ConveyorWaitObject (int);
	void ConveyorDone ();
	int WaitObjectWithVisionSensor (SensableObject*);
}


#endif