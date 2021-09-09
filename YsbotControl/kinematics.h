// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef __KINEMATICS_H__
#define __KINEMATICS_H__

#include <windows.h>


//////////////////////////////////////////////////////////////////////////////
// Definitions of Calling Convention & Storage-Class Attributes

#define ROBOT_V6_CALL		__stdcall

#ifdef ROBOT_V6_EXPORTS
	#define ROBOT_V6_API __declspec(dllexport)
#else
	#define ROBOT_V6_API __declspec(dllimport)
#endif


//////////////////////////////////////////////////////////////////////////////
// Type definitions

// D-H parameters
typedef struct _SCARA_PARAM
{
	double  a1;
	double  a2;
	double  d1;
	double  d4;
} SCARA_PARAM;

typedef struct _SCARA5DOF_PARAM
{
	double  a1;
	double  a2;
	double  a5;
	double  d1;
	double  d4;
} SCARA5DOF_PARAM;

typedef struct _DELTA_PARAM
{
	double  f;
	double  e;
	double  rf;
	double  re;
} DELTA_PARAM;

typedef struct _STACKER_PARAM
{
	double  a0;
	double  a1;
	double  a2;
	double  a3;
	double  a4;
	double  a5;
	double  origin_offset_x;
} STACKER_PARAM;

typedef struct _DELTA5DOF_PARAM
{
	double  l5;
	double  l6;
	double  l7;
	double  l8;
	double  l9;
	double  l10;
	//double  origin_offset_z;
} DELTA5DOF_PARAM;

typedef struct _CART5DOF_PARAM
{
	double  d4;
	double  d5;
} CART5DOF_PARAM;

typedef struct _DH6DOF_PARAM
{
	double  a1;
	double  a2;
	double  a3;
	double  d3;
	double  d4;
	double  d6;
	double  origin_offset_z;
} DH6DOF_PARAM;

typedef struct _BYD_PARAM
{
	double  a1;
	double  a2;
	double  a3;
	double  a5;
	double  d4;
	double  d6;
	double  origin_offset_z;
} BYD_PARAM;

typedef struct _PUNCH_PARAM
{
	double  a1;
	double  a2;
	double  d1;
	double  d4;
} PUNCH_PARAM;

typedef struct _DH_PARAM
{
	double  a0;
	double  a1;
	double  a2;
	double  a3;
	double  a4;
	double  a5;
	double  d1;
	double  d2;
	double  d3;
	double  d4;
	double  d5;
	double  d6;
	double  origin_offset_x;
	double  origin_offset_z;
} DH_PARAM;

typedef struct _DH_PARAM_EX
{
	double J34_CoupleRatio;
	double J45_CoupleRatio;
	double J56_CoupleRatio;
	double J46_CoupleRatio;
	struct TOOL
	{
		double  X_Offset;
		double  Y_Offset;
		double  Z_Offset;
		double  RX_Offset;
		double  RY_Offset;
		double  RZ_Offset;
	}Tool;
	struct WORK
	{
		double  X_Offset;
		double  Y_Offset;
		double  Z_Offset;
		double  RX_Offset;
		double  RY_Offset;
		double  RZ_Offset;
	}Work;
} DH_PARAM_EX;

typedef struct _TEACH_POINT
{
	double  x; // mm
	double  y; // mm
	double  z; // mm

	double  rx; // rad
	double  ry; // rad
	double  rz; // rad
} TEACH_POINT;

typedef struct _TEACH_JPOINT
{
	double  q1; // rad
	double  q2; // rad
	double  q3; // rad
	double  q4; // rad
	double  q5; // rad
	double  q6; // rad
} TEACH_JPOINT;

//////////////////////////////////////////////////////////////////////////////
// Exported functions

// D-H parameters

ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetScaraParam(const SCARA_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetScaraParam(SCARA_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetStackerParam(const STACKER_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetStackerParam(STACKER_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Set5DofScaraParam(const SCARA5DOF_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Get5DofScaraParam(SCARA5DOF_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetDeltaParam(const DELTA_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetDeltaParam(DELTA_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Set5DofDeltaParam(const DELTA_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Get5DofDeltaParam(DELTA_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Set6DofDHParam(const DH6DOF_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Get6DofDHParam(DH6DOF_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetBYDParam(const BYD_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetBYDParam(BYD_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Set5DofCartParam(const CART5DOF_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_Get5DofCartParam(CART5DOF_PARAM* pdhp, WORD wGroupIndex = 0);

ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetDHParam(const DH_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetDHParam(DH_PARAM* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetDHParamEx(const DH_PARAM_EX& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetDHParamEx(DH_PARAM_EX* pdhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetPunchParam(const PUNCH_PARAM& dhp, WORD wGroupIndex = 0);
ROBOT_V6_API void   ROBOT_V6_CALL rbt_GetPunchParam(PUNCH_PARAM* pdhp, WORD wGroupIndex = 0);

// Customize kinematics transformation rules
ROBOT_V6_API int    ROBOT_V6_CALL rbt_SetKinematicTrans(BOOL bSet, WORD wGroupIndex = 0);

// Kinematic transformation

ROBOT_V6_API int    ROBOT_V6_CALL rbt_FwdKinematics_V6(
			const double* pdfJntPos, // [in, size_is(6)]
			double* pdfCrtPos,       // [out, size_is(6)]
			DWORD* pdwPosture,       // [out]
			WORD wGroupIndex = 0
			);

ROBOT_V6_API int    ROBOT_V6_CALL rbt_InvKinematics_V6(
			const double* pdfCrtPos, // [in, size_is(6)]
			DWORD dwPosture,         // [in]
			double* pdfJntPos,       // [out, size_is(6)]
			WORD wGroupIndex = 0
			);


// Retrieve current JOINT/CARTESIAN positions
ROBOT_V6_API int    ROBOT_V6_CALL rbt_GetCurJPos(
			double *j0,
			double *j1,
			double *j2,
			double *j3,
			double *j4,
			double *j5,
			double *j6,
			double *j7,
			WORD wGroupIndex = 0
			);

ROBOT_V6_API int    ROBOT_V6_CALL rbt_GetCurCPos(
			double *x,
			double *y,
			double *z,
			double *rx,
			double *ry,
			double *rz,
			double *a,
			double *b,
			DWORD *pdwPosture,
			WORD wGroupIndex = 0
			);


//nType = 0: Stacker(LP130/LP180), 
//nType = 1: Stacker2(ShanDong), 
//nType = 2: 4-Dof Delta, 
//nType = 3: 5-Dof Delta, 
//nType = 4: 4-Dof Scara, 
//nType = 5: 5-Dof Scara, 
//nType = 6: 6-Dof DH Robot(GR985), 
//nType = 7: 6-Dof DH Robot(BYD Wirst Offset), 
//nType = 8: 5-Dof Cart Robot(3 + 2 Mechine Tool), 
//nType = 9: 4-Dof Cart Robot(Stacker3), 
//nType = 10: 6-Dof Robot(Stacker4), 
//nType = 11: 6-Dof Robot(Stacker5), 
//nType = 12: 4-Dof Scara Robot(Punch), 
//nType Delault = 0 
ROBOT_V6_API void   ROBOT_V6_CALL rbt_SetRobotType(int nType, WORD wGroupIndex = 0);

// Cone Motion
ROBOT_V6_API int	ROBOT_V6_CALL rbt_Cone_V6(
			double dfBaseVectX,
			double dfBaseVectY,
			double dfBaseVectZ,
			double dfTheta, // [in] degree, +/- stands for direction
			WORD wGroupIndex = 0
			);


// XY-Perpendicular RX/RY/RZ
ROBOT_V6_API int	ROBOT_V6_CALL rbt_RX_V6(
			double dfTheta, // [in] degree, +/- stands for direction
			WORD wGroupIndex = 0
			);

ROBOT_V6_API int	ROBOT_V6_CALL rbt_RY_V6(
			double dfTheta, // [in] degree, +/- stands for direction
			WORD wGroupIndex = 0
			);

ROBOT_V6_API int	ROBOT_V6_CALL rbt_RZ_V6(
			double dfTheta, // [in] degree, +/- stands for direction
			WORD wGroupIndex = 0
			);

ROBOT_V6_API int    ROBOT_V6_CALL rbt_LineWave(
							double dfX,
							double dfY,
							double dfZ,
							double dfRx,
							double dfRy,
							double dfRz,
							double dfFrequency,
							double dfAmplitude,
							double dfDelayTime,
							DWORD posture,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = 0x3F
							);

ROBOT_V6_API int    ROBOT_V6_CALL rbt_ArcWave(
							double x0, // 1st ref. point for x axis
							double y0, // 1st ref. point for y axis
							double z0, // 1st ref. point for z axis
							double x1, // target point for x axis
							double y1, // target point for y axis
							double z1, // target point for z axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							double dfFrequency,
							double dfAmplitude,
							double dfDelayTime,
							DWORD posture,
							WORD  wGroupIndex = 0,
							DWORD dwAxisMask = 0x3F
							);

ROBOT_V6_API int    ROBOT_V6_CALL rbt_CircleWave(
							double x0, // 1st ref. point for x axis
							double y0, // 1st ref. point for y axis
							double z0, // 1st ref. point for z axis
							double x1, // 2nd ref. point for x axis
							double y1, // 2nd ref. point for y axis
							double z1, // 2nd ref. point for z axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							double dfFrequency,
							double dfAmplitude,
							double dfDelayTime,
							DWORD posture,
							WORD  wGroupIndex = 0,
							DWORD dwAxisMask = 0x3F
							);


ROBOT_V6_API int    ROBOT_V6_CALL rbt_SetToolCordinate(
							          TEACH_POINT p1,
							          TEACH_POINT p2,
							          TEACH_POINT p3,
							          TEACH_POINT p4,
							          TEACH_POINT p5,
							          TEACH_POINT p6,
									  WORD    wGroupIndex = 0);

ROBOT_V6_API TEACH_POINT    ROBOT_V6_CALL rbt_GetToolCordinate(
							          TEACH_JPOINT p1,
							          TEACH_JPOINT p2,
							          TEACH_JPOINT p3,
							          TEACH_JPOINT p4,
							          TEACH_JPOINT p5,
							          TEACH_JPOINT p6,
									  WORD    wGroupIndex = 0);

#endif //__KINEMATICS_H__
