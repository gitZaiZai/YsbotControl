// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef  _kinematics_base_h
#define  _kinematics_base_h

#include "PosInfo.h"
#include "DHParaBase.h"

#define POST_ARM_LEFTY			0x01 // otherwise, arm righty
#define POST_ELBOW_BELOW		0x02 // otherwise, elbow above
#define POST_NONFLIP			0x04 // otherwise, flip
#define POST_J6DOUBLE			0x08 // otherwise, J6 single
#define POST_J4DOUBLE			0x10 // otherwise, J4 single

#define MAX_SOLUTION_NUM		(8)
#define TOLERANCE				(0.01)
#define SINGULARITY_RANGE		(M_PI * 3.0 / 180)
#define MAX_JOINT_SPEED			(1.00 * M_PI) // radian/sec

typedef struct _EXTENSION
{
	DWORD   last_posture;
	double  pre_J5;
	double  pre_pre_J5;

} EXTENSION;

DWORD  DeterminePosture(BOOL bArmLefty, int nJointNum, const double* pdfJointAngle, double dh_thredshold = 1.57 );
void __stdcall V6_InitExtension(void* pvExtension);
void __stdcall V6_PreProcessMotionCommand(int nGroup,
	int nSynAxisNum,
	const double* pdfCurCPos,
	const double* pdfTgtCPos,
	double* pdfOffset,
	void* pvExtension);

#endif