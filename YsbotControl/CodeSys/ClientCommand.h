// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _client_command_
#define _client_command_

#include "IecBasicTypes.h"

namespace Ysbot {

	enum IecCommandType {
		IEC_CMD_ILLEGAL   = 0,
		IEC_CMD_GROUPHALT,
		IEC_CMD_GROUPSTOP,
		IEC_CMD_GROUPINTERRUPT,
		IEC_CMD_GROUPCONTINUE,
		IEC_CMD_GROUPENABLE,
		IEC_CMD_GROUPDISABLE,
		IEC_CMD_GROUPSETOVERRIDE,
		IEC_CMD_PTPABS,
		IEC_CMD_PTPREL,
		IEC_CMD_LINABS,
		IEC_CMD_LINREL,
		IEC_CMD_CIRABS,
		IEC_CMD_CIRREL,
		IEC_CMD_JOGGROUP,
		IEC_CMD_JOGAXIS,
		IEC_CMD_POWERON,
		IEC_CMD_POWEROFF,
		IEC_CMD_RESETGROUP,
		IEC_CMD_SETPOS,
		IEC_CMD_SETCOORD,
		IEC_CMD_STOPJOG,
		IEC_CMD_SETBUFFERSIZE, // ERROR
		IEC_CMD_DIGITALOUTPUT,
		IEC_CMD_DIGITALINPUT,
		IEC_CMD_SETROBOTTYPE,
		IEC_CMD_SETDIGITALOUTPUT,
		IEC_CMD_WAITFINISHED,
		IEC_CMD_WAITINPUT,
		IEC_CMD_DELAY,
		IEC_CMD_CLEARENCODER,
		IEC_CMD_SETMAXPATHSPEED,
		IEC_CMD_SETMAXJOINTSPEED,
		IEC_CMD_TEST1, 
		IEC_CMD_CONVEYORBEGIN,
		IEC_CMD_CONVEYORWAIT,
		IEC_CMD_CONVEYORDONE,
		IEC_CMD_CONVEYOREND,
 		IEC_CMD_MAX_COMMANDS
	};

	enum SMC_ORIENTATION_MODE{

		GreatCircle,
		Axis
	};

	enum SMC_CIRC_MODE{
		BORDER,
		CENTER,
		RADIUS
	};

	enum MC_CIRC_PATHCHOICE {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	enum MC_BUFFER_MODE { 
		Aborting,
		Buffered,
		BlendingLow,
		BlendingPrevious,
		BlendingNext,
		BlendingHigh
	};

	enum MC_TRANSITION_MODE {

		TMNone,
		TMStartVelocity,
		TMCornerDistance
	};

#pragma pack(push) //保存对齐状态
// #if USE_MACHINE_X86
//   #pragma pack(4)//设定为4字节对齐
// #endif
#ifdef USE_MACHINE_X86
	#pragma pack(4)//设定为4字节对齐
#endif // USE_MACHINE_X86
	typedef struct _tagClientCommand
	{
		unsigned int         iCommandIndex;
		unsigned int         iSourceLineNo;

		short                CmdType; // 2 bytes

		SMC_POS_REF          EndPos;
		short                CoordSystem;

		short                BufferMode;
		short				 TransMode;
		double               TransParam[2]; 
 
		double				 Velocity;
		double				 Acceleration;
		double				 Deceleration;
		double				 Jerk;

		double				 VelFactor;
		double				 AccFactor;
		double				 JerkFactor;

		bool				 bState;  // power on or off.
		unsigned int         nMaxElapsedTime;

		SMC_POS_REF          RelDistance ;  // relative motion, line or ptp
		short				 OrientIPOMode;   

		SMC_POS_REF          CircAuxPos; // circle motion
		short				 CircMode;
		short                CircPathChoice;

		WORD				 wFig;
		SMC_POS_REF			 AuxAxisPos;  

		WORD				 wJogAxisMark; // jog motion.
		WORD				 wJogDirMark; 
		double				 dHaltDec;     // stop and halt.

		bool                 bSendDirect;

        WORD				 wPortValue;
        WORD				 wPortIndex;

		double               dfParams[10];
		int                  iParams[10];
 
		_tagClientCommand()
		{
			iCommandIndex   = 0;
			CmdType         = IEC_CMD_ILLEGAL;
			CoordSystem     = ACS;
			BufferMode		= Buffered;
			TransMode       = TMNone;
 
			Velocity        = 10.0;
			Acceleration    = 100.0;
			Deceleration    = 100.0;
			Jerk            = 1000.0;

			VelFactor       = 0.5;
			AccFactor       = 0.5;
			JerkFactor      = 0.5;
			nMaxElapsedTime = 0;
			bSendDirect     = false;
 		}

	}ClientCommand, *LPClientCommand;

#pragma pack(pop) //恢复对齐状态

}
#endif
