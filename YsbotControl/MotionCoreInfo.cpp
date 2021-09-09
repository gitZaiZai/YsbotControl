// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "MotionCoreInfo.h"

using namespace std;

MotionCoreInfo::MotionCoreInfo() throw ()
{
	sprintf( errString, "" );

	for (int i=0; i<MAX_JOINT_NUM; i++)
	{
		dfPos[i]         = 0;
		dfAxisSpeed[i]   = 0;
 	}

	for ( int i = 0; i < 10; ++i )
	{
		dfMemData[i] = 0.0;
		iMemData[i] = 0;
	}

	driver_alarm = 0;
	nCmdType = -1;
	nCommandIndex = -1;
	nCommandCount = 0;
	nPulseStockCount = 0;
	dfFeedSpeed   = 0.0;
	iMotionStatus = msSTOP; // stop
	emergency_stop = false;
	nHomeStatus = 0;
	dfCurFeedSpeed = 0; 
}

const MotionCoreInfo& MotionCoreInfo::operator= (const MotionCoreInfo& src) throw ()
{
	sprintf( errString,"%s", src.errString );

	for (int i=0; i<MAX_JOINT_NUM; i++)
	{
		dfPos[i]         = src.dfPos[i];
		dfAxisSpeed[i]   = src.dfAxisSpeed[i];
	}
 
	for ( int i = 0; i < 10; ++i )
	{
		dfMemData[i] = src.dfMemData[i];
		iMemData[i] = src.iMemData[i];
	}

	driver_alarm  = src.driver_alarm;
	nCmdType = src.nCmdType;
	nCommandIndex = src.nCommandIndex;
	dfFeedSpeed   = src.dfFeedSpeed;
	iMotionStatus = src.iMotionStatus; // stop
	nCommandCount = src.nCommandCount;
	nPulseStockCount = src.nPulseStockCount;

	nHomeStatus = src.nHomeStatus;
	dfCurFeedSpeed = src.dfCurFeedSpeed;
	emergency_stop = src.emergency_stop;

	return *this;
}
