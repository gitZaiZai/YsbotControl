// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _group_info
#define _group_info

#include "GroupProperties.h"

struct MotionCoreInfo
{ 
	// 0 GMS_RUNNING:  executing a motion command
	// 1 GMS_STOP:     stopping status and none of command exist in a command queue
	// 2 GMS_HOLD:     holding status (because usig MCC_HoldMotion())  and some commands exist in a command queue
	// 3 GMS_DELAYING: delay status   (because usig MCC_DelayMotion()) and some commands exist in a command queue
	enum MotionStatus {
		msRUNNING =0,        
		msSTOP =1,          
		msHOLD =2,         
		msDELAYING =3    
	};
	MotionCoreInfo() throw ();
	MotionCoreInfo (const MotionCoreInfo& gi ) throw () { operator= (gi); }
	const MotionCoreInfo& operator= (const MotionCoreInfo&) throw ();
	
	int     nCmdType; //  0: Pont to Point
	//  1: Line
	//  2: Clockwise Arc/Circle
	//  3: Counterclockwise Arc/Circle
	//  6: Delay
	//  7: Enable Blend
	//  8: Disable Blend
	//  9: Enable In-Position
	// 10: Disable In-Position
	unsigned long long     nCommandIndex;
	int     nCommandCount;
	int     nPulseStockCount;
	double  dfFeedSpeed;
	double  dfPos[MAX_JOINT_NUM];
	unsigned long driver_alarm;
	MotionStatus     iMotionStatus;
	char    errString[MAX_ERRSTR_LEN];

	int     nHomeStatus;
	double  dfCurFeedSpeed;
	double  dfAxisSpeed[MAX_JOINT_NUM];
	bool    emergency_stop;
	double  dfMemData[10];
	double  iMemData[10];
};

#endif

