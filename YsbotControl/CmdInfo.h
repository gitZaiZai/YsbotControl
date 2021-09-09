// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#ifndef _command_infomation_h
#define _command_infomation_h

#include "PosInfo.h"
#include "RobotState.h"
#include "GroupProperties.h"
 
class Dynamic :  public BasicScriptVariable
{
public:
	Dynamic();
	Dynamic (const Dynamic& dyn) { operator= (dyn); }
 	const Dynamic& operator= (const Dynamic& dyn); 
	
	double  PathVel;  //	λ���ٶ�(mm/s)
	double	PathAcc;  //	λ�ü��ٶ�(mm/s^2)
	double	PathDec;  //	λ�ü��ٶ�(mm/s^2)
	double  PathJerk;

	double	JointVel; //	�ؽ��ٶ�(%)
	double	JointAcc; //	�ؽڼ��ٶ�(%)
	double	JointDec; //	�ؽڼ��ٶ�(%)
	double  JointJerk;

	double	OriVel;   //	��ת�ٶ�(deg/s)
	double	OriAcc;   //	��ת���ٶ�(deg/s^2)
	double	OriDec;   //    ��ת���ٶ�(deg/s^2)
	double  OriJerk;

	double  Override;
	bool    custom_parameter;

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	void  check_value( const char* name, const bool, char* );

};

std::ostream& operator<< (std::ostream& os, const Dynamic& v);
std::istream& operator>> (std::istream &in, Dynamic& v);

class Overlap : public BasicScriptVariable
{
public:
	enum BufferMode 
	{ 
		mcAborting = 0,
		mcBuffered = 1,
		mcBlendingLow = 2,
		mcBlendingPrevious = 3,
		mcBlendingNext = 4,
		mcBlendingHigh = 5,
	};
 	enum TransitionMode 
	{ 
		TMNone = 0,
		TMStartVelocity = 1,
		TMCornerDistance = 2,
		TMConstantVelocity = 3, // invalid for codesys below
		TMMaxCornerDeviation = 4, 
		TMConstDeviation = 5,
		TMIntermediatePoint = 6,
		TMDynamicOptimized = 7,
	};	
	Overlap();
	Overlap (const Overlap& ove) { operator= (ove); }
	const Overlap& operator= (const Overlap& ove); 

	BufferMode          iBufferMode; 
	TransitionMode      iTransMode;
	Real                TransitionParameter[3];

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	void  check_value( const char* name, const bool, char* );

};
std::ostream& operator<< (std::ostream& os, const Overlap& v);
std::istream& operator>> (std::istream &in, Overlap& v);

enum CommandT 
{ 
	CMD_NULL = 0,  
	CMD_JOG,  
	CMD_PTP,
	CMD_LIN,  
 	CMD_CIR,    
	CMD_PTPREL,
	CMD_LINREL,
	CMD_CIRREL,
	CMD_SPLINE,  
	CMD_WAITIO, // add by dan.
	CMD_DELAYING,
	CMD_HALT,
	CMD_CONTINUE,
	CMD_ABORT,
	CMD_ABORTJOG,
	CMD_HOME,
	CMD_ABORTHOME,
	CMD_READINPUT,
	CMD_SETOUTPUT,
	CMD_SETPOSITION,
	CMD_RESET,
	CMD_POWER,
	CMD_BLEND,
	CMD_ERRORSTOP,
	CMD_RESETCMDINDEX,
	CMD_TIMEDELAY,
	CMD_MCOMMAND,
	CMD_SETOVERRIDE,
	CMD_WAITMOTIONFINISH,
	CMD_SETCOORDREF,
	CMD_CONVEYORBEGIN,
	CMD_CONVEYORWAIT,
	CMD_CONVEYORDONE,
	CMD_CONVEYOREND,
};
// different variables that are used by the different possible commands
// only the variables that are related to the current commandType have
// legal values
class CmdInfo
{
public:

	int      iObjNo;
	int      iSourceLineNo;
	CommandT iCommandType;

    // in degree
 	PosInfo     startPos;  // ACS, MCS, PCS
	PosInfo     destPos;
 	PosInfo     auxPos; // for circle motion
 
	const CartRef*  cartref; // user coordinate system, may be dynamic coordinate 
	const CartPos*  tool;    // tool coordinate system 

	Dynamic  dyn;  
	Overlap  ovl;
    Real     dec_time;

	unsigned short  HomeMark;
 	
	int            iInOutNo;
	int            iInOutBit;
	int            iIoValue;
 
	int            chJogAxisMark;
	int            chJogDirMark;

	long int       iDelayTime;

	JogCoordT      iJogCoordType;

	bool           customStartPos;
	bool           bOnOff;

public:
	CmdInfo(void);
	~CmdInfo(void);
	const CmdInfo& operator= (const CmdInfo& ) throw (); 
	CmdInfo (const CmdInfo& cf) throw () { operator= (cf); }

};

#endif